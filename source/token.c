/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [token.c]
 - Alexander Brandt 2020

https://en.wikipedia.org/wiki/Ascii
https://en.wikipedia.org/wiki/UTF-8

TODO: Investigate how handle CRLF correctly?
      discard CR seems so wrong...
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "japan-buffer.h"
#include "japan-string.h"
#include "japan-token.h"


struct jaTokenizer
{
	// Set at creation:
	int (*callback)(struct jaTokenizer*, struct jaToken*);
	const uint8_t* input;
	const uint8_t* input_end;

	struct jaStatus st;

	// Zero at creation:
	struct jaBuffer token_buffer;
	struct jaBuffer end_buffer;

	size_t line_number;
	size_t unit_number;
	size_t byte_offset;

	uint64_t end; // A internal copy of what the user receives
};


static inline uint64_t sTranslateEnds(uint32_t code)
{
	switch (code)
	{
	// Old ASCII
	case 0x00: return JA_TOKEN_END_NULL;
	case 0x0A: return JA_TOKEN_END_NEW_LINE;

	case 0x09: return JA_TOKEN_END_WHITESPACE; // Space
	case 0x20: return JA_TOKEN_END_WHITESPACE; // Horizontal Tab

	case 0x21: return JA_TOKEN_END_EXCLAMATION;
	case 0x22: return JA_TOKEN_END_QUOTATION;
	case 0x23: return JA_TOKEN_END_HASH;
	case 0x24: return JA_TOKEN_END_DOLLAR;
	case 0x25: return JA_TOKEN_END_PERCENT;
	case 0x26: return JA_TOKEN_END_AMPERSAND;
	case 0x27: return JA_TOKEN_END_APOSTHOPHE;
	case 0x28: return JA_TOKEN_END_PARENTHESIS_L;
	case 0x29: return JA_TOKEN_END_PARENTHESIS_R;
	case 0x2A: return JA_TOKEN_END_ASTERISK;
	case 0x2B: return JA_TOKEN_END_PLUS;
	case 0x2C: return JA_TOKEN_END_COMMA;
	case 0x2D: return JA_TOKEN_END_MINUS;
	case 0x2E: return JA_TOKEN_END_FULL_STOP;
	case 0x2F: return JA_TOKEN_END_SLASH;
	case 0x3A: return JA_TOKEN_END_COLON;
	case 0x3B: return JA_TOKEN_END_SEMICOLON;
	case 0x3C: return JA_TOKEN_END_LESS_THAN;
	case 0x3D: return JA_TOKEN_END_EQUALS;
	case 0x3E: return JA_TOKEN_END_GREATER_THAN;
	case 0x3F: return JA_TOKEN_END_QUESTION;
	case 0x40: return JA_TOKEN_END_AT;
	case 0x5B: return JA_TOKEN_END_SQUARE_BRACKET_L;
	case 0x5C: return JA_TOKEN_END_BACKSLASH;
	case 0x5D: return JA_TOKEN_END_SQUARE_BRACKET_R;
	case 0x5E: return JA_TOKEN_END_ACCENT;
	case 0x60: return JA_TOKEN_END_GRAVE_ACCENT;
	case 0x7B: return JA_TOKEN_END_CURLY_BRACKET_L;
	case 0x7C: return JA_TOKEN_END_VERTICAL_LINE;
	case 0x7D: return JA_TOKEN_END_CURLY_BRACKET_R;
	case 0x7E: return JA_TOKEN_END_TILDE;
	default: break;
	}

	return 0;
}


static inline int sBufferAppend(struct jaBuffer* buffer, size_t* index, uint8_t data, struct jaStatus* st)
{
	if (*index + 1 > buffer->size)
	{
		if (jaBufferResize(buffer, *index + 1) == NULL)
		{
			jaStatusSet(st, "jaTokenize", JA_STATUS_MEMORY_ERROR, NULL);
			return 1;
		}
	}

	((uint8_t*)buffer->data)[*index] = data;
	*index += 1;

	return 0;
}


static int sASCIITokenizer(struct jaTokenizer* state, struct jaToken* out_token)
{
	size_t token_buffer_i = 0;
	size_t end_buffer_i = 0;

	uint64_t end_bit = 0;
	state->end = 0;

	if (state->input >= state->input_end)
		return 2;

	out_token->line_number = state->line_number;
	out_token->unit_number = state->unit_number;
	out_token->byte_offset = state->byte_offset;

	for (; state->input < state->input_end; state->input += 1)
	{
		if (jaASCIIValidateUnit(*state->input) != 0)
		{
			jaStatusSet(&state->st, "jaTokenize", JA_STATUS_ASCII_ERROR, "character \\%02X", *state->input);
			return 1;
		}

		if (*state->input == 0x0D) // CR
			continue;

		state->unit_number += 1;
		state->byte_offset += 1;

		// Choose which bytes mark the token end, and which ones forms it
		if ((end_bit = sTranslateEnds((uint32_t)*state->input)) != 0)
		{
			// Following ends require some fine grinding
			switch (*state->input)
			{
			case 0x0A: // LF
				state->line_number += 1;
				break;

			case 0x20: // Space
			case 0x09: // Horizontal Tab
				state->end |= end_bit;
				goto next_character; // Do not append

			case 0x00: // NULL
				state->end |= end_bit;
				state->input_end = state->input; // User gave us a wrong end
				goto outside_loop;               // Stop the world
			}

			// Append end to respective buffer
			if (sBufferAppend(&state->end_buffer, &end_buffer_i, *state->input, &state->st) != 0)
				return 1;

			state->end |= end_bit;
		}
		else
		{
			// Byte to form the token
			if (state->end == 0)
				sBufferAppend(&state->token_buffer, &token_buffer_i, *state->input, &state->st);
			else
			{
				state->unit_number -= 1; // This byte never existed ;)
				state->byte_offset -= 1;
				goto outside_loop;
			}
		}

	next_character:;
	}

outside_loop:
	sBufferAppend(&state->token_buffer, &token_buffer_i, 0x00, &state->st);
	sBufferAppend(&state->end_buffer, &end_buffer_i, 0x00, &state->st);

	// Bye!
	out_token->string = state->token_buffer.data;
	out_token->end_string = state->end_buffer.data;
	out_token->end = state->end;
	return 0;
}


static int sUTF8Tokenizer(struct jaTokenizer* state, struct jaToken* out_token)
{
	(void)state;
	(void)out_token;
	return 1;
}


// ----


struct jaTokenizer* jaASCIITokenizerCreate(const uint8_t* string, size_t n)
{
	struct jaTokenizer* state = NULL;

	if ((state = calloc(1, sizeof(struct jaTokenizer))) != NULL)
	{
		state->callback = sASCIITokenizer;
		state->input_end = string + n;
		state->input = string;

		jaStatusSet(&state->st, "jaTokenize", JA_STATUS_SUCCESS, NULL);
	}

	return state;
}

struct jaTokenizer* jaUTF8TokenizerCreate(const uint8_t* string, size_t n)
{
	struct jaTokenizer* state = NULL;

	if ((state = calloc(1, sizeof(struct jaTokenizer))) != NULL)
	{
		state->callback = sUTF8Tokenizer;
		state->input_end = string + n;
		state->input = string;

		jaStatusSet(&state->st, "jaTokenize", JA_STATUS_SUCCESS, NULL);
	}

	return state;
}

inline void jaTokenizerDelete(struct jaTokenizer* state)
{
	if (state != NULL)
	{
		jaBufferClean(&state->token_buffer);
		jaBufferClean(&state->end_buffer);
		free(state);
	}
}

inline int jaTokenize(struct jaTokenizer* state, struct jaToken* out_token, struct jaStatus* st)
{
	if (out_token != NULL)
	{
		int ret = state->callback(state, out_token);
		jaStatusCopy(&state->st, st);
		return ret;
	}

	return 1;
}
