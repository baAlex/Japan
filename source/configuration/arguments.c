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


inline void jaConfigurationArguments(struct jaConfiguration* config, enum jaEncode encode, int argc, const char* argv[])
{
	jaConfigurationArgumentsEx(config, encode, JA_SKIP_FIRST, NULL, argc, argv);
}

void jaConfigurationArgumentsEx(struct jaConfiguration* config, enum jaEncode encode, enum jaArgumentsFlags flags,
                                void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*), int argc,
                                const char* argv[])
{
	struct jaDictionaryItem* item = NULL;

	for (int i = (flags == JA_PARSE_FIRST) ? 0 : 1; i < argc; i++)
	{
		// Check key
		if (argv[i][0] != '-')
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_EXPECTED_KEY_TOKEN, i, argv[i], NULL);

			continue;
		}

		if ((item = jaDictionaryGet((struct jaDictionary*)config, (argv[i] + 1))) == NULL)
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_EXPECTED_KEY_TOKEN, i, argv[i], NULL);

			continue;
		}

		// Retrieve value
		if ((i + 1) == argc)
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_NO_ASSIGNMENT, i, NULL, argv[i] + 1);

			break;
		}

		// Store
		enum jaStatusCode code = Store(item->data, argv[i + 1], SET_BY_ARGUMENTS);

		if (code != JA_STATUS_SUCCESS)
		{
			if (warnings_callback != NULL)
				warnings_callback(code, i, argv[i + 1], argv[i] + 1);
		}

		i++; // Important!
	}
}
