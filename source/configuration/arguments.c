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

 [arguments.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "private.h"


/*-----------------------------

 jaConfigurationArguments()
-----------------------------*/
int jaConfigurationArguments(struct jaConfiguration* config, int argc, const char* argv[], struct jaStatus* st)
{
	return jaConfigurationArgumentsEx(config, argc, argv, ARGUMENTS_DEFAULT, st);
}

int jaConfigurationArgumentsEx(struct jaConfiguration* config, int argc, const char* argv[],
                               enum jaArgumentsFlags flags, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;

	for (int i = (flags == ARGUMENTS_INCLUDE_FIRST_ONE) ? 0 : 1; i < argc; i++)
	{
		// Check key
		if (argv[i][0] != '-')
		{
			JA_DEBUG_PRINT("[Warning] Unexpected token '%s'\n", (argv[i]));
			continue;
		}

		if ((item = jaDictionaryGet((struct jaDictionary*)config, (argv[i] + 1))) == NULL)
		{
			JA_DEBUG_PRINT("[Warning] Unknown configuration '%s'\n", (argv[i] + 1));
			continue;
		}

		// Check value
		if ((i + 1) == argc)
		{
			JA_DEBUG_PRINT("[Warning] No value for configuration '%s'\n", (argv[i] + 1));
			break;
		}

		// Set!
		Store(item->data, argv[i + 1], SET_BY_FILE); // TODO

		i++; // Important!
	}

#ifdef JA_DEBUG
	jaDictionaryIterate((struct jaDictionary*)config, PrintCallback, NULL);
#endif

	return 0; // TODO?
}
