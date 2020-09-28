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

#if 0

#include "private.h"


uint64_t TranslateEnds(uint32_t code)
{
	switch (code)
	{
	// Old ASCII
	case 0x00: return JA_DELIMITER_NULL;
	case 0x0A: return JA_DELIMITER_NEW_LINE;

	case 0x09: return JA_DELIMITER_WHITESPACE; // Space
	case 0x20: return JA_DELIMITER_WHITESPACE; // Horizontal Tab

	case 0x21: return JA_DELIMITER_EXCLAMATION;
	case 0x22: return JA_DELIMITER_QUOTATION;
	case 0x23: return JA_DELIMITER_HASH;
	case 0x24: return JA_DELIMITER_DOLLAR;
	case 0x25: return JA_DELIMITER_PERCENT;
	case 0x26: return JA_DELIMITER_AMPERSAND;
	case 0x27: return JA_DELIMITER_APOSTHOPHE;
	case 0x28: return JA_DELIMITER_PARENTHESIS_L;
	case 0x29: return JA_DELIMITER_PARENTHESIS_R;
	case 0x2A: return JA_DELIMITER_ASTERISK;
	case 0x2B: return JA_DELIMITER_PLUS;
	case 0x2C: return JA_DELIMITER_COMMA;
	case 0x2D: return JA_DELIMITER_MINUS;
	case 0x2E: return JA_DELIMITER_FULL_STOP;
	case 0x2F: return JA_DELIMITER_SLASH;
	case 0x3A: return JA_DELIMITER_COLON;
	case 0x3B: return JA_DELIMITER_SEMICOLON;
	case 0x3C: return JA_DELIMITER_LESS_THAN;
	case 0x3D: return JA_DELIMITER_EQUALS;
	case 0x3E: return JA_DELIMITER_GREATER_THAN;
	case 0x3F: return JA_DELIMITER_QUESTION;
	case 0x40: return JA_DELIMITER_AT;
	case 0x5B: return JA_DELIMITER_SQUARE_BRACKET_L;
	case 0x5C: return JA_DELIMITER_BACKSLASH;
	case 0x5D: return JA_DELIMITER_SQUARE_BRACKET_R;
	case 0x5E: return JA_DELIMITER_ACCENT;
	case 0x60: return JA_DELIMITER_GRAVE_ACCENT;
	case 0x7B: return JA_DELIMITER_CURLY_BRACKET_L;
	case 0x7C: return JA_DELIMITER_VERTICAL_LINE;
	case 0x7D: return JA_DELIMITER_CURLY_BRACKET_R;
	case 0x7E: return JA_DELIMITER_TILDE;
	default: break;
	}

	return 0;
}


int BufferAppendByte(struct jaBuffer* buffer, size_t* cursor, uint8_t data, struct jaStatus* st)
{
	if (*cursor + 1 > buffer->size)
	{
		if (jaBufferResize(buffer, *cursor + 1) == NULL)
		{
			jaStatusSet(st, "jaTokenize", JA_STATUS_MEMORY_ERROR, NULL);
			return 1;
		}
	}

	((uint8_t*)buffer->data)[*cursor] = data;
	*cursor += 1;

	return 0;
}


int BufferAppend(struct jaBuffer* buffer, size_t* cursor, const void* data, size_t n, struct jaStatus* st)
{
	if (*cursor + n > buffer->size)
	{
		if (jaBufferResize(buffer, *cursor + n) == NULL)
		{
			jaStatusSet(st, "jaTokenize", JA_STATUS_MEMORY_ERROR, NULL);
			return 1;
		}
	}

	memcpy((uint8_t*)buffer->data + *cursor, data, n);
	*cursor += n;

	return 0;
}


struct jaTokenizer* jaTokenizerCreateString(enum jaStringEncode encode, const uint8_t* string, size_t n)
{
	struct jaTokenizer* state = NULL;

	if (string == NULL)
		return NULL;

	if ((state = calloc(1, sizeof(struct jaTokenizer))) != NULL)
	{
		state->encode = encode;
		state->file = NULL;

		state->input = string;
		state->input_end = string + n;

		jaStatusSet(&state->st, "jaTokenize", JA_STATUS_SUCCESS, NULL);
	}

	return state;
}


struct jaTokenizer* jaTokenizerCreateFile(enum jaStringEncode encode, FILE* file)
{
	struct jaTokenizer* state = NULL;

	if (file == NULL)
		return NULL;

	if ((state = calloc(1, sizeof(struct jaTokenizer) + FILE_BUFFER_LEN)) != NULL)
	{
		state->encode = encode;
		state->file = file;

		state->input = NULL;
		state->input_end = NULL;

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
		jaBufferClean(&state->start_buffer);
		free(state);
	}
}


inline struct jaToken* jaTokenize(struct jaTokenizer* state, struct jaStatus* st)
{
	int ret = 1;

	if (state->encode == JA_UTF8)
		ret = (state->file == NULL) ? UTF8Tokenizer(state) : UTF8FileTokenizer(state);
	else
		ret = (state->file == NULL) ? ASCIITokenizer(state) : ASCIIFileTokenizer(state);

	jaStatusCopy(&state->st, st);
	return (ret == 0) ? &state->user : NULL;
}

#endif
