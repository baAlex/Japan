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
 - Alexander Brandt 2019
-----------------------------*/

#include "private.h"


/*-----------------------------

 jaConfigReadArguments()
-----------------------------*/
void jaConfigReadArguments(struct jaConfig* config, int argc, const char* argv[], enum jaConfigArgumentsFlags flags)
{
	struct jaDictionaryItem* item = NULL;
	struct Cvar* cvar = NULL;
	union Value old_value;

	for (int i = (flags == CONFIG_IGNORE_FIRST) ? 1 : 0; i < argc; i++)
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
		cvar = item->data;
		old_value = cvar->value;

		if ((i + 1) == argc)
		{
			JA_DEBUG_PRINT("[Warning] No value for configuration '%s'\n", (argv[i] + 1));
			break;
		}

		switch (cvar->type)
		{
		case TYPE_INT:
			if (StoreInt(&cvar->value.i, argv[i + 1], cvar->min.i, cvar->max.i) != 0)
				JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a integer value as '%s' requires\n",
				               argv[i + 1], (argv[i] + 1));
			break;
		case TYPE_FLOAT:
			if (StoreFloat(&cvar->value.f, argv[i + 1], cvar->min.f, cvar->max.f) != 0)
				JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a decimal value as '%s' requires\n",
				               argv[i + 1], (argv[i] + 1));
			break;
		case TYPE_STRING: StoreString(&cvar->value.s, argv[i + 1]);
		}

		if (memcmp(&old_value, &cvar->value, sizeof(union Value)) != 0) // Some change?
			cvar->set_by = SET_BY_ARGUMENTS;

		i++; // Important!
	}

#ifdef JA_DEBUG
	JA_DEBUG_PRINT("(jaConfigReadArguments)\n");
	jaDictionaryIterate((struct jaDictionary*)config, PrintCallback, NULL);
#endif
}
