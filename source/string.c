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

 [string.c]
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


inline int jaASCIIValidateUnit(uint8_t byte)
{
	return (byte <= 127) ? 0 : 1;
}


int jaASCIIValidateString(const uint8_t* string, size_t n, size_t* out_bytes)
{
	size_t lenght = 0;

	for (const uint8_t* string_end = (string + n); string < string_end; string++)
	{
		if (jaASCIIValidateUnit(*string) != 0)
			break;

		lenght += 1;

		if (*string == 0x00) // NULL
		{
			if (out_bytes != NULL)
				*out_bytes = lenght; // Counts NULL as part of the string

			return 0;
		}
	}

	if (out_bytes != NULL)
		*out_bytes = lenght; // Valid bytes so far

	return 1;
}


// 🌏👨‍🚀 Wait, UTF8 is variable-width encoded? 🔫👩‍🚀 Always has ��


inline size_t jaUTF8UnitLength(uint8_t head_byte)
{
	// High-bits indicates how many tails the unit uses

	if ((head_byte >> 7) == 0) // Single byte unit
		return 1;
	else if ((head_byte >> 5) == 0x06) // 0x06 = 0b00000110, Two-bytes unit
		return 2;
	else if ((head_byte >> 4) == 0x0E) // 0x0E = 0b00001110, Three-bytes unit
		return 3;
	else if ((head_byte >> 3) == 0x1E) // 0x1E = 0b00011110, Four-bytes unit
		return 4;

	return 255;
}

int jaUTF8ValidateUnit(const uint8_t* byte, size_t n, size_t* out_unit_len, uint32_t* out_unit_code)
{
	size_t unit_len = jaUTF8UnitLength(*byte);

	if (out_unit_len != NULL)
		*out_unit_len = unit_len;

	// Single byte unit
	// Nothing more to do in an old ASCII character
	if (unit_len == 1)
	{
		if (out_unit_code != NULL)
			*out_unit_code = (uint32_t)(*byte);

		return 0;
	}

	// Unicode don't use more than 4 bytes
	else if (unit_len > 4)
		return 1;

	// Our lovely user provided us a truncated unit
	else if (unit_len > n)
		return 1;

	// Validate whole unit
	{
		uint32_t code = (uint32_t)(*byte & (0xFF >> (unit_len + 1))); // Cap head byte
		byte += 1;

		// Concatenate tail bytes
		switch (unit_len - 1)
		{
		case 3:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 2:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 1:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
		}

		// Overloading check
		// Is possible to encode in a longer unit that the needed
		if ((code < 0x0080 && unit_len == 2)      // Two bytes unit = U+0080 to U+07FF
		    || (code < 0x0800 && unit_len == 3)   // Three bytes unit = U+0800 to U+FFFF
		    || (code < 0x10000 && unit_len == 4)) // Four bytes unit = U+10000 to U+10FFFF
			return 1;

		// UTF-16 invalid codes (surrogates)
		// To keep compatibility allowing conversions
		else if (code >= 0xD800 && code <= 0xDFFF)
			return 1;

		// Last Unicode code
		else if (code > 0x10FFFF) // Planes 15–16, F0000–​10FFFF: 'Supplementary Private Use Area planes'
			return 1;

		// Bye!
		if (out_unit_code != NULL)
			*out_unit_code = code;
	}

	return 0;
}

static inline int sUTF8ValidateUnitSimple(const uint8_t* byte, const uint8_t* end, size_t* unit_len)
{
	*unit_len = jaUTF8UnitLength(*byte);

	if (*unit_len == 1)
		return 0;
	else if (*unit_len > 4 || (byte + *unit_len) > end)
		return 1;

	// Validate whole unit
	{
		uint32_t code = (uint32_t)(*byte & (0xFF >> (*unit_len + 1)));
		byte += 1;

		switch (*unit_len - 1)
		{
		case 3:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 2:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 1:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
		}

		if ((code < 0x0080 && *unit_len == 2)     // Two bytes unit = U+0080 to U+07FF
		    || (code < 0x0800 && *unit_len == 3)  // Three bytes unit = U+0800 to U+FFFF
		    || (code < 0x10000 && *unit_len == 4) // Four bytes unit = U+10000 to U+10FFFF
		    || (code >= 0xD800 && code <= 0xDFFF) // UTF-16 invalid codes
		    || (code > 0x10FFFF))                 // Last Unicode code
			return 1;
	}

	return 0;
}

int jaUTF8ValidateString(const uint8_t* string, size_t n, size_t* out_bytes, size_t* out_units)
{
	size_t bytes = 0;
	size_t units = 0;

	size_t unit_lenght = 0;

	for (const uint8_t* string_end = (string + n); string < string_end; string++)
	{
		if (sUTF8ValidateUnitSimple(string, string_end, &unit_lenght) != 0)
			break;

		bytes += unit_lenght;
		units += 1;

		if (*string == 0x00) // NULL
		{
			if (out_bytes != NULL)
				*out_bytes = bytes; // Counts NULL as part of the string
			if (out_units != NULL)
				*out_units = units;

			return 0;
		}

		string += unit_lenght - 1;
	}

	if (out_bytes != NULL)
		*out_bytes = bytes; // Valid bytes so far
	if (out_units != NULL)
		*out_units = units;

	return 1;
}


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
		jaStatusCopy(&state->st, st);
		return state->callback(state, out_token);
	}

	return 1;
}
