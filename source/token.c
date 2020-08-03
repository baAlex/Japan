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

	union
	{
		struct jaTokenEnd e; // A internal copy of what the user receives
		uint64_t union_hack; // Hack of the union variety
	} end;
};


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

	state->end.union_hack = 0;

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

		// Close your eyes, hopefully an intelligent compiler will append
		// the following conditional mess to the big switch below (TODO)
		if ((*state->input >= 0x21 && *state->input <= 0x2F) || (*state->input >= 0x3A && *state->input <= 0x40) ||
		    (*state->input >= 0x5B && *state->input <= 0x5E) || (*state->input == 0x60) ||
		    (*state->input >= 0x7B && *state->input <= 0x7E) || (*state->input == 0x0A))
		{
			if (sBufferAppend(&state->end_buffer, &end_buffer_i, *state->input, &state->st) != 0)
				return 1;
		}

		// Choose which characters will form the token,
		// and which ones mark his end
		switch (*state->input)
		{
		case 0x21: state->end.e.exclamation = true; break;
		case 0x22: state->end.e.quotation = true; break;
		case 0x23: state->end.e.number_sign = true; break;
		case 0x24: state->end.e.dollar_sign = true; break;
		case 0x25: state->end.e.percent_sign = true; break;
		case 0x26: state->end.e.ampersand = true; break;
		case 0x27: state->end.e.apostrophe = true; break;
		case 0x28: state->end.e.left_parenthesis = true; break;
		case 0x29: state->end.e.right_parenthesis = true; break;
		case 0x2A: state->end.e.asterisk = true; break;
		case 0x2B: state->end.e.plus_sign = true; break;
		case 0x2C: state->end.e.comma = true; break;
		case 0x2D: state->end.e.minus_sign = true; break;
		case 0x2E: state->end.e.full_stop = true; break;
		case 0x2F: state->end.e.slash = true; break;

		case 0x3A: state->end.e.colon = true; break;
		case 0x3B: state->end.e.semicolon = true; break;
		case 0x3C: state->end.e.less_than_sign = true; break;
		case 0x3D: state->end.e.equals_sign = true; break;
		case 0x3E: state->end.e.greater_than_sign = true; break;
		case 0x3F: state->end.e.question_mark = true; break;
		case 0x40: state->end.e.at_sign = true; break;

		case 0x5B: state->end.e.left_square_bracket = true; break;
		case 0x5C: state->end.e.backslash = true; break;
		case 0x5D: state->end.e.right_square_bracket = true; break;
		case 0x5E: state->end.e.accent = true; break;

		case 0x60: state->end.e.grave_accent = true; break;

		case 0x7B: state->end.e.left_curly_bracket = true; break;
		case 0x7C: state->end.e.vertical_line = true; break;
		case 0x7D: state->end.e.right_curly_bracket = true; break;
		case 0x7E: state->end.e.tilde = true; break;

		case 0x20: // Space
		case 0x09: // Horizontal Tab
			state->end.e.whitespace = true;
			break;

		case 0x0A: // LF
			state->end.e.new_line = true;
			state->line_number += 1;
			break;

		case 0x00: // NULL
			state->end.e.null = true;
			state->input_end = state->input; // User gave us a wrong end
			goto outside_loop;

		// Character to form the token
		default:
			if (state->end.union_hack == 0)
				sBufferAppend(&state->token_buffer, &token_buffer_i, *state->input, &state->st);
			else
			{
				state->unit_number -= 1; // This character never existed ;)
				state->byte_offset -= 1;
				goto outside_loop;
			}
		}
	}

outside_loop:
	sBufferAppend(&state->token_buffer, &token_buffer_i, 0x00, &state->st);
	sBufferAppend(&state->end_buffer, &end_buffer_i, 0x00, &state->st);

	// Bye!
	out_token->string = state->token_buffer.data;
	out_token->end_string = state->end_buffer.data;
	out_token->end = state->end.e;
	return 0;
}


static int sUTF8Tokenizer(struct jaTokenizer* state, struct jaToken* out_token)
{
	(void)state;
	(void)out_token;
	return 1;
}


static inline struct jaTokenizer* sTokenizerCreate(const uint8_t* string, size_t n,
                                                   int (*callback)(struct jaTokenizer*, struct jaToken*))
{
	struct jaTokenizer* state = NULL;

	if ((state = calloc(1, sizeof(struct jaTokenizer))) != NULL)
	{
		state->callback = callback;
		state->input_end = string + n;
		state->input = string;

		jaStatusSet(&state->st, "jaTokenize", JA_STATUS_SUCCESS, NULL);
	}

	return state;
}


struct jaTokenizer* jaASCIITokenizerCreate(const uint8_t* string, size_t n)
{
	return sTokenizerCreate(string, n, sASCIITokenizer);
}

struct jaTokenizer* jaUTF8TokenizerCreate(const uint8_t* string, size_t n)
{
	return sTokenizerCreate(string, n, sUTF8Tokenizer);
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
