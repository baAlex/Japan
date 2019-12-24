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

	// Private:
	bool comment;
	bool literal;
	bool advance_a_line;
};


static int sTokenize(FILE* fp, struct jaBuffer* buffer, struct TokenizerState* state, struct jaStatus* st)
{
	// TODO: multi-line:
	// https://gcc.gnu.org/onlinedocs/gcc-3.2/cpp/Initial-processing.html
	// https://stackoverflow.com/questions/27792508/ruby-backslash-to-continue-string-on-a-new-line

	size_t cursor = 0;
	char ch = '\0';

	if (state->advance_a_line == true)
		state->line_number += 1;

	state->advance_a_line = false;
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
					continue;
				else
					break;
			}

			if (ch != '\n' && (ch == '#' || state->comment == true))
			{
				state->comment = true;
				continue;
			}
		}

		// Add character to the token
		if (ch == '\n')
		{
			state->comment = false;

			if (state->literal == true) // TODO
			{
				jaStatusSet(st, "jaConfigReadFile", STATUS_UNSUPPORTED_FEATURE,
				            "(Line %i) Multi-line literals unsupported", state->line_number + 1);
				return 1;
			}

			// New line characters are the exception to the norm, they
			// are considered tokens on themself when alone. This to
			// allow the correct count of line numbers by the caller if
			// want to do it (also, makes the tokenization more easy)
			if (cursor == 0)
			{
				// In the 'bye' procedure this end being an {'\0', '\0'}
				state->token[cursor] = '\0';
				cursor++;
			}

			state->advance_a_line = true;
			break;
		}
		else
		{
			// Note than a normal character didn't do a break
			state->token[cursor] = ch;
			cursor++;
		}
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

		if (s.token[0] != '\0') // Tokenizer traits new lines as empty strings
			printf("%s%04i: \"%s\"\n", (s.comment) ? "#" : "", s.line_number + 1, s.token);
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
