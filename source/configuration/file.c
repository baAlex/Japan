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

#include "japan-token.h"
#include "private.h"


#define CVAR_MAX_NAME 64


static inline void sStageZeroFailure(struct jaToken* t, const char* name)
{
	if ((t->end_delimiters & JA_DELIMITER_NEW_LINE))
	{
		if ((t->end_delimiters &
		     ~(JA_DELIMITER_WHITESPACE | JA_DELIMITER_NEW_LINE | JA_DELIMITER_EQUALS | JA_DELIMITER_QUOTATION)) != 0)
		{
			printf("[Error] Unexpected characters after '%s', line %zu\n", name, t->line_number + 1);
		}
		else if ((t->end_delimiters & JA_DELIMITER_EQUALS))
		{
			printf("[Error] Assignment for '%s' should take place in the same line, line %zu\n", name,
			       t->line_number + 1);
		}
		else if (!(t->end_delimiters & JA_DELIMITER_EQUALS))
			printf("[Error] No assignment after '%s', line %zu\n", name, t->line_number + 1);
		else
			printf("[Error] Line ends before the assignment of '%s', line %zu\n", name, t->line_number + 1);
	}
	else
	{
		if (t->end_delimiters == JA_DELIMITER_WHITESPACE)
			printf("[Error] No assignment after '%s', line %zu\n", name, t->line_number + 1);
		else
			printf("[Error] Unexpected characters after '%s', line %zu\n", name, t->line_number + 1);
	}
}


static int sParse(struct jaTokenizer* tknzr, struct jaConfiguration* config,
                  void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*), struct jaStatus* st)
{
	struct jaToken* t = NULL;
	struct jaCvar* cvar = NULL;

	char name[CVAR_MAX_NAME] = {0};
	int stage = 0;

	jaStatusSet(st, "jaConfigurationFile", JA_STATUS_SUCCESS, NULL);

	while (1)
	{
		if ((t = jaTokenize(tknzr, st)) == NULL)
			break;

		// Retrieve variable name
		if (stage == 0)
		{
			strcat(name, (char*)t->string); // FIXME, a bomb ready to explode!

			if (t->end_delimiters == JA_DELIMITER_FULL_STOP)
			{
				strcat(name, ".");
				continue;
			}

			// Only a combination of WHITESPACE, EQUALS and QUOTE, in the correct
			// order, is considered a valid assignment
			if (t->end_string[0] == '=' &&
			    (t->end_delimiters == (JA_DELIMITER_WHITESPACE | JA_DELIMITER_EQUALS) ||
			     t->end_delimiters == (JA_DELIMITER_WHITESPACE | JA_DELIMITER_EQUALS | JA_DELIMITER_QUOTATION)))
			{
				if ((cvar = jaCvarGet(config, name)) == NULL)
				{
					printf("[Error] Unknown variable '%s', line %zu\n", name, t->line_number + 1);
					stage = -1;
				}
				else
					stage = 1;
			}

			// Nope, an incorrect assignment
			else
			{
				sStageZeroFailure(t, name);
				stage = -1;
			}
		}

		// Retrieve value
		else if (stage == 1)
		{
			memset(name, 0, CVAR_MAX_NAME);
			stage = 0;
		}

		// Whatever stage, if the token ends with NEW_LINE this marks
		// the end of the statement (a new start if some error happened)
		if ((t->end_delimiters & JA_DELIMITER_NEW_LINE))
		{
			if (stage != -1) // A previous stage set this and already printed a message
				printf("[Error] Statement incomplete, line %zu\n", t->line_number + 1);

			memset(name, 0, CVAR_MAX_NAME);
			stage = 0;
		}
	}

	if (st->code != JA_STATUS_SUCCESS)
		return 1;

	// Bye!
	return 0;
}


int jaConfigurationFile(struct jaConfiguration* config, const char* filename, struct jaStatus* st)
{
	FILE* fp = NULL;
	int ret = 0;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaConfigurationFile", JA_STATUS_IO_ERROR, NULL);
		return 1;
	}

	ret = jaConfigurationFileEx(config, fp, NULL, st);
	fclose(fp);

	return ret;
}


int jaConfigurationFileEx(struct jaConfiguration* config, FILE* fp,
                          void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*),
                          struct jaStatus* st)
{
	struct jaTokenizer* tknzr = NULL;
	int ret = 0;

	if ((tknzr = jaTokenizerCreateFile(JA_ASCII, fp)) == NULL)
	{
		jaStatusSet(st, "jaConfigurationFile", JA_STATUS_IO_ERROR, NULL);
		return 1;
	}

	ret = sParse(tknzr, config, warnings_callback, st);
	jaTokenizerDelete(tknzr);

	return ret;
}
