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

#if 0

#include "private.h"


int ASCIITokenizer(struct jaTokenizer* state)
{
	size_t token_buffer_cursor = 0;
	size_t end_buffer_cursor = 0;

	uint64_t end_bit = 0;

	if (state->input >= state->input_end)
		return 2;

	// Some bits that the user receives,
	// most of them from the previous iteration
	state->user.line_number = state->line_number;
	state->user.unit_number = state->unit_number;
	state->user.byte_offset = state->byte_offset;
	state->user.start_delimiters = state->end_delimiters;

	if (jaBufferCopy(&state->start_buffer, &state->end_buffer) != 0)
	{
		jaStatusSet(&state->st, "ASCIITokenizer", JA_STATUS_MEMORY_ERROR, NULL);
		return 1;
	}

	// Cycle trough units
	state->end_delimiters = 0;

	for (; state->input < state->input_end; state->input += 1)
	{
		if (jaUnitValidateASCII(*state->input) != 0)
		{
			jaStatusSet(&state->st, "ASCIITokenizer", JA_STATUS_ASCII_ERROR, "character \\%02X", *state->input);
			return 1;
		}

		state->unit_number += 1;
		state->byte_offset += 1;

		if (*state->input == 0x0D) // CR
			continue;

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
				state->end_delimiters |= end_bit;
				goto next_character; // Do not append

			case 0x00: // NULL
				state->end_delimiters |= end_bit;
				state->input_end = state->input; // User gave us a wrong end
				goto outside_loop;               // Stop the world
			}

			// Append end to respective buffer
			if (BufferAppendByte(&state->end_buffer, &end_buffer_cursor, *state->input, &state->st) != 0)
				return 1;

			state->end_delimiters |= end_bit;
		}
		else
		{
			// Byte to form the token
			if (state->end_delimiters == 0)
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

	// Bye!, remaining information for the user
	state->user.string = state->token_buffer.data;
	state->user.start_string = state->start_buffer.data;
	state->user.end_string = state->end_buffer.data;
	state->user.end_delimiters = state->end_delimiters;

	return 0;
}


int ASCIIFileTokenizer(struct jaTokenizer* state)
{
	if (state->input == NULL || state->input >= state->input_end)
	{
		size_t n = fread(((struct jaTokenizer*)state + 1), 1, FILE_BUFFER_LEN, state->file);

		if (n == 0)
		{
			if (feof(state->file) != 0) // End reached?
				return 2;

			// Nope, an error
			jaStatusSet(&state->st, "ASCIIFileTokenizer", JA_STATUS_IO_ERROR, NULL);
			return 1;
		}

		state->input = (uint8_t*)((struct jaTokenizer*)state + 1);
		state->input_end = state->input + n;
	}

	return ASCIITokenizer(state);
}

#endif
