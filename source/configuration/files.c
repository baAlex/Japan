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
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "private.h"


struct TokenizerState
{
	char* token;
	int line_number;

	struct
	{
		bool eof : 1;
		bool break_ws : 1;
		bool break_nl : 1;
		bool break_sc : 1;
	};

	// Private:
	int sum_line_number;
	char return_artificial;
	char append_artificial;
};

struct ParserState
{
	struct jaDictionaryItem* item_found;
	bool equal_found;

	bool buguous_statement; // "Buguous" is even a word?
};


/*-----------------------------

 sTokenize()
-----------------------------*/
static int sTokenize(FILE* fp, struct jaBuffer* buffer, struct TokenizerState* state, struct jaStatus* st)
{
	// Unnecessary over-commented because string manipulation in C is hard...
	// and I know that I going to forgot how all this works.

	size_t cursor = 0;
	char ch = '\0';

	bool in_literal = false;
	bool in_comment = false;

	state->break_ws = false;
	state->break_nl = false;
	state->break_sc = false;

	state->token = buffer->data;

	// The previous token was the last one in the previous line?
	if (state->sum_line_number != 0)
	{
		state->line_number += state->sum_line_number;
		state->sum_line_number = 0;
	}

	// The previous token was broken by a character that now
	// we need to return as a token itself
	if (state->return_artificial != '\0')
	{
		state->token[0] = state->return_artificial;
		cursor++;

		state->return_artificial = '\0';

		// We lie to the caller about this artificial token
		// being broken by a white space. As soon the following
		// big loop finds the first character of the next token
		// the artificial symbol is returned first
		state->break_ws = true;
	}

	// Similar to the above case. The previous token was
	// broken by a character, but that now we need to
	// append in the future token to return
	if (state->append_artificial != '\0')
	{
		state->token[0] = state->append_artificial;
		cursor += 1; // TODO, memory error?

		// If was a quote character, lets set the variable
		// that indicate being inside a literal
		if (state->append_artificial == '\"')
			in_literal = true;

		state->append_artificial = '\0';
	}

	// Build the future token to return, iterating one
	// character at time
	while (1)
	{
		// Check for memory, and read
		if (buffer->size < (cursor + 1))
		{
			if (jaBufferResize(buffer, (cursor + 1)) == NULL)
			{
				jaStatusSet(st, "jaConfigurationFile", STATUS_MEMORY_ERROR, NULL);
				return 1;
			}

			state->token = buffer->data;
		}

		if (fread(&ch, sizeof(char), 1, fp) != 1)
		{
			if (feof(fp) != 0)
			{
				state->eof = true;
				break;
			}

			jaStatusSet(st, "jaConfigurationFile", STATUS_UNEXPECTED_EOF, NULL);
			return 1;
		}

		// Is a quote character?
		if (ch == '\"' && in_comment == false)
		{
			if (in_literal == false)
			{
				if (cursor == 0)
					in_literal = true;
				else
				{
					// We can't open the literal on a token that
					// already has content (cursor != 0). It is
					// necessary to break here

					// Lets indicate the next call to append
					// a quote character
					state->append_artificial = '\"';
					state->break_ws = true; // The above lie
					break;
				}
			}
			else
			{
				// Add quote character to the token
				state->token[cursor] = ch;
				cursor++;

				state->break_ws = true; // The above lie
				in_literal = false;
				continue;
			}
		}

		if (in_literal == false)
		{
			// White space character
			if (ch == ' ' || ch == '\t')
			{
				// In case of initial whitespace (cursor == 0), we
				// ignore it by continuing with the loop whitout
				// break the token

				if (cursor != 0)
					state->break_ws = true; // Is not a lie here!

				continue;
			}

			// A semicolon, the same procedure that the white
			// space character above
			else if (ch == ';')
			{
				if (cursor != 0)
					state->break_sc = true;

				continue;
			}

			// Comment characer procedure, we set the variable indicating
			// the state, and then ignoring everything (in_comment == true)
			// except for the '\n' character that we can't continue over
			else if (ch != '\n' && (ch == '#' || in_comment == true))
			{
				in_comment = true;
				continue;
			}

			// Equal symbol procedure, similar to the quote character, we
			// break here but indicating to return a character the next
			// call rather to append one
			else if (ch == '=')
			{
				state->break_ws = true; // Lie!

				if (cursor == 0)
				{
					state->token[cursor] = ch;
					cursor++;
					break;
				}

				state->return_artificial = '=';
				break;
			}
		}

		// End of line procedure
		if (ch == '\n')
		{
			in_comment = false;

			if (in_literal == true) // TODO?
			{
				jaStatusSet(st, "jaConfigurationFile", STATUS_UNSUPPORTED_FEATURE,
				            "(Line %i) Multi-line literals unsupported", state->line_number + 1);
				return 1;
			}

			if (cursor == 0) // An empty line, just with '\n'
				state->line_number += 1;
			else
			{
				state->sum_line_number += 1;
				state->break_nl = true; // Not a lie
			}

			continue;
		}

		// And finally, the most important character, basically all
		// those that didn't fall under previous conditions
		if (state->break_nl == false && state->break_sc == false && state->break_ws == false)
		{
			state->token[cursor] = ch;
			cursor++;
		}
		else
		{
			// This one is hacky, most procedures above didn't do
			// a break (except for those that return artificial
			// characters) rather, they set some "break_" variable

			// So, here a new token start but following "break_"
			// variables we need to return the current token content
			// and postpone the actual character to be appended next call
			state->append_artificial = ch;
			break;
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

 sParse()
-----------------------------*/
static void sParse(const struct TokenizerState* tknzr, struct jaConfiguration* config, struct ParserState* state)
{
	if (state->buguous_statement == false)
	{
		// First step, find an valid configuration variable
		// In case of failure the statement is set as buguous
		if (state->item_found == NULL)
		{
			if ((state->item_found = jaDictionaryGet((struct jaDictionary*)config, tknzr->token)) == NULL)
			{
				JA_DEBUG_PRINT(
				    "[Warning] Expected an configuration key at line %i, found the unknown token '%s' instead\n",
				    tknzr->line_number + 1, tknzr->token);

				state->buguous_statement = true;
			}
		}
		else
		{
			// Second step, check existence of an equal symbol
			if (state->equal_found == false)
			{
				if (tknzr->token[0] == '=')
					state->equal_found = true;
				else
				{
					JA_DEBUG_PRINT(
					    "[Warning] Expected an equal sign for key '%s' in line %i, found token '%s' instead\n",
					    state->item_found->key, tknzr->line_number + 1, tknzr->token);

					state->buguous_statement = true; // As above
				}
			}

			// Third step, validate value and assign it
			else
			{
				// Value isn't the last token in the statement!
				if (tknzr->break_sc != true && tknzr->break_nl != true)
				{
					JA_DEBUG_PRINT("[Warning] Statement in line %i isn't properly closed, '%s' assignament ignored\n",
					               tknzr->line_number + 1, state->item_found->key);
					state->buguous_statement = true;
				}
				else
				{
					// JA_DEBUG_PRINT(" ~~~~~ '%s' = '%s' ~~~~~ \n", state->item_found->key, tknzr->token);
					Store(state->item_found->data, tknzr->token, SET_BY_FILE); // TODO
				}

				// And back to step one!
				state->item_found = NULL;
				state->equal_found = false;
			}
		}
	}

	// There is a semicolon or a new line after this token,
	// lets indicate this so the next step do a fresh start
	if (tknzr->break_sc == true || tknzr->break_nl == true)
	{
		// The third step was never reached, warn about that
		if (state->buguous_statement == false && (state->item_found != NULL))
			JA_DEBUG_PRINT("[Warning] Configuration '%s' in line %i did't have a value assigned\n",
			               state->item_found->key, tknzr->line_number + 1);

		// Set
		state->buguous_statement = false;
		state->item_found = NULL;
		state->equal_found = false;
	}
}


/*-----------------------------

 jaConfigurationFile()
-----------------------------*/
int jaConfigurationFile(struct jaConfiguration* config, const char* filename, struct jaStatus* st)
{
	FILE* fp = NULL;
	int ret_value = 0;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaConfigurationFile", STATUS_IO_ERROR, NULL);
		return 1;
	}

	ret_value = jaConfigurationFileEx(config, fp, FILE_DEFAULT, st);

	fclose(fp);
	return ret_value;
}

int jaConfigurationFileEx(struct jaConfiguration* config, FILE* fp, enum jaFileFlags flags, struct jaStatus* st)
{
	struct jaBuffer buffer = {0};
	struct TokenizerState tknzr = {0};
	struct ParserState prsr = {0};

	jaStatusSet(st, "jaConfigurationFile", STATUS_SUCCESS, NULL);

	while (1)
	{
		if (sTokenize(fp, &buffer, &tknzr, st) != 0)
			goto return_failure;

		if (flags == FILE_TOKENIZE_ONLY) {}
		else
			sParse(&tknzr, config, &prsr);

		// printf("(b:%s%s%s) %04i: \"%s\"\n", (tknzr.break_ws) ? "ws" : "--", (tknzr.break_nl) ? "nl" : "--",
		//       (tknzr.break_sc) ? "sc" : "--", tknzr.line_number + 1, tknzr.token);

		// Token is the last one
		if (tknzr.eof == true)
			break;
	}

	// Bye!
#ifdef JA_DEBUG
	jaDictionaryIterate((struct jaDictionary*)config, PrintCallback, NULL);
#endif

	jaBufferClean(&buffer);
	return 0;

return_failure:
	jaBufferClean(&buffer);
	return 1;
}
