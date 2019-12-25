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

 [file.c]
 - Alexander Brandt 2019
-----------------------------*/

#include "private.h"


struct TokenizerState
{
	char* token;
	int line_number;

	bool eof;

	bool break_ws;
	bool break_nl;
	bool break_sc;

	bool initial_ws;
	bool initial_nl;
	bool initial_sc;

	// Private:
	bool comment;
	bool literal;
	bool advance_line;
	bool ret_fake_equal;
};


static int sTokenize(FILE* fp, struct jaBuffer* buffer, struct TokenizerState* state, struct jaStatus* st)
{
	// TODO: multi-line:
	// https://gcc.gnu.org/onlinedocs/gcc-3.2/cpp/Initial-processing.html
	// https://stackoverflow.com/questions/27792508/ruby-backslash-to-continue-string-on-a-new-line

	size_t cursor = 0;
	char ch = '\0';

	state->break_ws = false;
	state->break_nl = false;
	state->break_sc = false;

	state->initial_ws = false;
	state->initial_nl = false;
	state->initial_sc = false;

	if (state->advance_line == true)
	{
		state->line_number += 1;
		state->advance_line = false;
	}

	if (state->ret_fake_equal == true)
	{
		// The previous token contain an equal symbol, but it didn't
		// get returned as other characters where already there, and
		// the equal symbol is supposed to act as an separator... long
		// story short: in this call we need to return an fake "=" and
		// return immediately, lying about being break by a white space
		state->ret_fake_equal = false;
		state->break_ws = true;
		state->token = "=";
		return 0;
	}

	state->token = buffer->data;

	while (1)
	{
		// Check memory
		if (buffer->size < (cursor + 1))
		{
			if (jaBufferResize(buffer, (cursor + 1)) == NULL)
			{
				jaStatusSet(st, "jaConfigReadFile", STATUS_MEMORY_ERROR, NULL);
				return 1;
			}

			state->token = buffer->data;
		}

		// Read a character and determine if is suitable
		// to add into the token (check whitespaces, comments, literals...)
		if (fread(&ch, sizeof(char), 1, fp) != 1)
		{
			if (feof(fp) != 0)
			{
				state->eof = true;
				break;
			}

			jaStatusSet(st, "jaConfigReadFile", STATUS_UNEXPECTED_EOF, NULL);
			return 1;
		}

		if (ch == '\"' && state->comment == false)
		{
			if (state->literal == false)
				state->literal = true;
			else
				state->literal = false;
		}

		if (state->literal == false)
		{
			if (ch == ' ')
			{
				if (cursor == 0) // Initial whitespace
				{
					state->initial_ws = true;
					continue;
				}

				state->break_ws = true;
				break;
			}
			else if (ch == ';')
			{
				if (cursor == 0) // Initial semicolons
				{
					state->initial_sc = true;
					continue;
				}

				state->break_sc = true;
				break;
			}
			else if (ch != '\n' && (ch == '#' || state->comment == true))
			{
				state->comment = true;
				continue;
			}
			else if (ch == '=')
			{
				// Always break with the equal symbol, separating the current token
				// if is the case. The caller is notified as an white space happened.
				// While reciving an fake (or artificial) equal symbol the next call.
				state->break_ws = true;

				if (cursor == 0)
				{
					state->token[cursor] = ch;
					cursor++;
					break; // The 'bye' procedure appends the '\0'
				}

				state->ret_fake_equal = true;
				break;
			}
		}

		if (ch == '\n')
		{
			state->comment = false;

			if (state->literal == true)
			{
				// TODO
				jaStatusSet(st, "jaConfigReadFile", STATUS_UNSUPPORTED_FEATURE,
				            "(Line %i) Multi-line literals unsupported", state->line_number + 1);
				return 1;
			}

			if (cursor == 0) // An empty line, just with '\n'
			{
				state->line_number += 1;
				state->initial_nl = true;
				continue;
			}

			state->advance_line = true;
			state->break_nl = true;
			break;
		}

		// Add character to the token
		// Note than a normal character didn't do a break
		state->token[cursor] = ch;
		cursor++;
	}

	// Bye!
	if (cursor != 0)
		state->token[cursor] = '\0'; // :)
	else
		state->token = NULL;

	return 0;
}


/*-----------------------------

 jaConfigReadFile()
-----------------------------*/
int jaConfigReadFile(struct jaConfig* config, const char* filename, struct jaStatus* st)
{
	FILE* fp = NULL;
	struct jaBuffer buffer = {0};
	struct TokenizerState s = {0};

	jaStatusSet(st, "jaConfigReadFile", STATUS_SUCCESS, NULL);

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaConfigReadFile", STATUS_IO_ERROR, NULL);
		goto return_failure;
	}

	while (1)
	{
		// Get a token
		if (sTokenize(fp, &buffer, &s, st) != 0)
			goto return_failure;

		if (s.eof == true)
			break;

		printf("(b:%s%s%s, i:%s%s%s) %04i: \"%s\"\n", (s.break_ws) ? "ws" : "", (s.break_nl) ? "nl" : "",
		       (s.break_sc) ? "sc" : "", (s.initial_ws) ? "ws" : "--", (s.initial_nl) ? "nl" : "--",
		       (s.initial_sc) ? "sc" : "--", s.line_number + 1, s.token);
	}

	// Bye!
#ifdef JA_DEBUG
	JA_DEBUG_PRINT("(jaConfigReadFile, '%s')\n", filename);
	jaDictionaryIterate((struct jaDictionary*)config, PrintCallback, NULL);
#endif

	fclose(fp);
	jaBufferClean(&buffer);
	return 0;

return_failure:
	if (fp != NULL)
		fclose(fp);
	jaBufferClean(&buffer);
	return 1;
}
