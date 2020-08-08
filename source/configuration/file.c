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


static int sParse(struct jaTokenizer* tknzr, struct jaConfiguration* config, FILE* fp,
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
			strcat(name, (char*)t->string);

			// If the token ends with a single dot, concatenate
			// it with the previous one... and nothing more
			if (t->end == JA_END_FULL_STOP)
			{
				strcat(name, ".");
				continue;
			}

			// After all the concatenations, here, we should have the
			// entire variable name
			if ((cvar = jaCvarGet(config, name)) != NULL)
			{
				printf("[Yay!!] cvar '%s' found!, line %zu\n", name, t->line_number + 1);
				memset(name, 0, CVAR_MAX_NAME);
			}
			else
			{
				printf("[Error] '%s' is not a cvar, line %zu\n", name, t->line_number + 1);
				memset(name, 0, CVAR_MAX_NAME);
			}
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

	ret = sParse(tknzr, config, fp, warnings_callback, st);
	jaTokenizerDelete(tknzr);

	return ret;
}
