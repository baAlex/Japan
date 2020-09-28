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


static inline void sTrimWhitespaces(const char* string, char* output)
{
	bool body_reached = false;
	char* o = output;

	for (; *string != 0x00 && (o - output) < JA_CVAR_KEY_MAX_LEN; string++)
	{
		if (*string != 0x20 && *string != 0x09) // SPACE, TAB
		{
			body_reached = true;

			*o = *string;
			o += 1;
		}
		else
		{
			if (body_reached == true)
				break;
		}
	}

	*o = 0x00;
}


inline void jaConfigurationArguments(struct jaConfiguration* config, enum jaEncode encode, int argc, const char* argv[])
{
	jaConfigurationArgumentsEx(config, encode, JA_SKIP_FIRST, NULL, argc, argv);
}


void jaConfigurationArgumentsEx(struct jaConfiguration* config, enum jaEncode encode, enum jaArgumentsFlags flags,
                                void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*), int argc,
                                const char* argv[])
{
	struct jaDictionaryItem* item = NULL;
	char trimmed_key[JA_CVAR_KEY_MAX_LEN];

	for (int i = (flags == JA_PARSE_FIRST) ? 0 : 1; i < argc; i++)
	{
		// Check key
		sTrimWhitespaces(argv[i], trimmed_key);

		if (trimmed_key[0] != 0x2D) // HYPHEN
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_INVALID_KEY_TOKEN, i, trimmed_key, NULL);

			continue;
		}

		if ((item = jaDictionaryGet((struct jaDictionary*)config, (trimmed_key + 1))) == NULL)
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_EXPECTED_KEY_TOKEN, i, trimmed_key + 1, NULL);

			continue;
		}

		// Retrieve value
		if ((i + 1) == argc)
		{
			if (warnings_callback != NULL)
				warnings_callback(JA_STATUS_NO_ASSIGNMENT, i, trimmed_key + 1, NULL);

			break;
		}

		// Validate value
		if (encode == JA_UTF8)
		{
			if (jaUTF8ValidateString((uint8_t*)argv[i + 1], UINT_MAX, NULL, NULL) != 0)
			{
				if (warnings_callback != NULL)
					warnings_callback(JA_STATUS_UTF8_ERROR, i, trimmed_key + 1, argv[i + 1]);

				continue;
				i++; // Important!
			}
		}
		else
		{
			if (jaASCIIValidateString((uint8_t*)argv[i + 1], UINT_MAX, NULL) != 0)
			{
				if (warnings_callback != NULL)
					warnings_callback(JA_STATUS_ASCII_ERROR, i, trimmed_key + 1, argv[i + 1]);

				continue;
				i++; // Important!
			}
		}

		// Store
		enum jaStatusCode code = Store(item->data, argv[i + 1], SET_BY_ARGUMENTS);

		if (code != JA_STATUS_SUCCESS)
		{
			if (warnings_callback != NULL)
				warnings_callback(code, i, trimmed_key + 1, argv[i + 1]);
		}

		i++; // Important!
	}
}
