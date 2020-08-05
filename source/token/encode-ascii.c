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

 [encode-ascii.c]
 - Alexander Brandt 2020
-----------------------------*/

#include "private.h"


int ASCIITokenizer(struct jaTokenizer* state)
{
	size_t token_buffer_cursor = 0;
	size_t end_buffer_cursor = 0;

	uint64_t end_bit = 0;
	state->end = 0;

	if (state->input >= state->input_end)
		return 2;

	state->user.line_number = state->line_number;
	state->user.unit_number = state->unit_number;
	state->user.byte_offset = state->byte_offset;

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
		if ((end_bit = TranslateEnds((uint32_t)*state->input)) != 0)
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
			if (BufferAppendByte(&state->end_buffer, &end_buffer_cursor, *state->input, &state->st) != 0)
				return 1;

			state->end |= end_bit;
		}
		else
		{
			// Byte to form the token
			if (state->end == 0)
				BufferAppendByte(&state->token_buffer, &token_buffer_cursor, *state->input, &state->st);
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
	BufferAppendByte(&state->token_buffer, &token_buffer_cursor, 0x00, &state->st);
	BufferAppendByte(&state->end_buffer, &end_buffer_cursor, 0x00, &state->st);

	// Bye!
	state->user.string = state->token_buffer.data;
	state->user.end_string = state->end_buffer.data;
	state->user.end = state->end;
	return 0;
}
