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

 [options.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "dictionary.h"
#include "options.h"
#include "utilities.h"

enum SetBy
{
	SET_DEFAULT = 0,
	SET_BY_ARGUMENTS,
	SET_BY_FILE
};

enum Type
{
	TYPE_INT = 0,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_STRING
};

union Value {
	int i;
	float f;
	bool b;
	const char* s;
};

struct Option
{
	enum Type type;
	enum SetBy set_by;
	union Value value;

	union Value min;
	union Value max;
	struct Dictionary* possible_values;
};

#ifdef DEBUG
static void sPrintCallback(struct DictionaryItem* item, void* data)
{
	(void)data;
	struct Option* option = (struct Option*)item->data;

	switch (option->type)
	{
	case TYPE_INT:
		DEBUG_PRINT(" - %s = %i [i%s]\n", item->key, option->value.i, (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_FLOAT:
		DEBUG_PRINT(" - %s = %f [f%s]\n", item->key, option->value.f, (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_BOOL:
		DEBUG_PRINT(" - %s = %s [b%s]\n", item->key, (option->value.b == true) ? "true" : "false",
		            (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_STRING:
		DEBUG_PRINT(" - %s = \"%s\" [s%s]\n", item->key, option->value.s,
		            (option->set_by == SET_DEFAULT) ? "" : ", (*)");
	}
}
#endif


/*-----------------------------

 OptionsCreate()
-----------------------------*/
EXPORT inline struct Options* OptionsCreate()
{
	return (struct Options*)DictionaryCreate(NULL);
}


/*-----------------------------

 OptionsDelete()
-----------------------------*/
EXPORT inline void OptionsDelete(struct Options* options)
{
	DictionaryDelete((struct Dictionary*)options);
}


/*-----------------------------

 OptionsReadArguments()
-----------------------------*/
static inline void sStoreString(const char* org, const char** dest)
{
	// TODO
	*dest = org;
}

static inline int sStoreBool(const char* org, bool* dest)
{
	bool value = false;
	bool value_set = false;

	for (const char* c = org; *c != '\0'; c++)
	{
		if (value_set == false)
		{
			if (isspace((int)*c) == 0) // If not
			{
				if (strncmp(c, "true", strlen("true")) == 0)
				{
					value = true;
					value_set = true;
					c += strlen("true") - 1;
				}
				else if (strncmp(c, "false", strlen("false")) == 0)
				{
					value = false;
					value_set = true;
					c += strlen("false") - 1;
				}
				else
					return 1;
			}
		}
		else if (isspace((int)*c) == 0) // If not
			return 1;
	}

	*dest = value;
	return 0;
}

static inline int sStoreFloat(const char* org, float* dest, float min, float max)
{
	char* end = NULL;
	float value = strtof(org, &end); // "Past the last character interpreted"

	if (*end != '\0')
		for (; *end != '\0'; end++)
		{
			if (isspace((int)*end) == 0) // If not
				return 1;
		}

	*dest = ClampFloat(value, min, max);
	return 0;
}

static inline int sStoreInt(const char* org, int* dest, int min, int max)
{
	char* end = NULL;
	long value = strtol(org, &end, 0);

	if (*end != '\0')
		for (; *end != '\0'; end++)
		{
			if (isspace((int)*end) == 0)
			{
				// Try with float
				float temp = 0.0f;

				if (sStoreFloat(org, &temp, (float)min, (float)max) != 0)
					return 1;

				value = lroundf(temp);
				break;
			}
		}

	if (value > INT_MAX || value < INT_MIN)
		return 1;

	*dest = ClampInt((int)value, min, max);
	return 0;
}

EXPORT void OptionsReadArguments(struct Options* options, int argc, const char* argv[])
{
	struct DictionaryItem* item = NULL;
	struct Option* option = NULL;
	union Value old_value;

	for (int i = 1; i < argc; i++)
	{
		// Check key
		if (argv[i][0] != '-')
		{
			DEBUG_PRINT("[Warning] Unexpected token '%s'\n", (argv[i]));
			continue;
		}

		if ((item = DictionaryGet((struct Dictionary*)options, (argv[i] + 1))) == NULL)
		{
			DEBUG_PRINT("[Warning] Unknown option '%s'\n", (argv[i] + 1));
			continue;
		}

		// Check value
		option = item->data;
		memcpy(&old_value, &option->value, sizeof(union Value));

		if ((i + 1) == argc)
		{
			DEBUG_PRINT("[Warning] No value for option '%s'\n", (argv[i] + 1));

			i++; // Important!
			continue;
		}

		switch (option->type)
		{
		case TYPE_INT:
			if (sStoreInt(argv[i + 1], &option->value.i, option->min.i, option->max.i) != 0)
				DEBUG_PRINT("[Warning] Token '%s' can't be cast into a integer value as '%s' requires\n", argv[i + 1],
				            (argv[i] + 1));
			break;
		case TYPE_FLOAT:
			if (sStoreFloat(argv[i + 1], &option->value.f, option->min.f, option->max.f) != 0)
				DEBUG_PRINT("[Warning] Token '%s' can't be cast into a decimal value as '%s' requires\n", argv[i + 1],
				            (argv[i] + 1));
			break;
		case TYPE_BOOL:
			if (sStoreBool(argv[i + 1], &option->value.b) != 0)
				DEBUG_PRINT("[Warning] Token '%s' can't be cast into a boolean value as '%s' requires\n", argv[i + 1],
				            (argv[i] + 1));
			break;
		case TYPE_STRING: sStoreString(argv[i + 1], &option->value.s);
		}

		if (memcmp(&old_value, &option->value, sizeof(union Value)) != 0) // Something change?
			option->set_by = SET_BY_ARGUMENTS;

		i++; // Important!
	}

#ifdef DEBUG
	DEBUG_PRINT("(OptionsReadArguments)\n");
	DictionaryIterate((struct Dictionary*)options, sPrintCallback, NULL);
#endif
}


/*-----------------------------

 OptionsReadFile()
-----------------------------*/
EXPORT int OptionsReadFile(struct Options* options, const char* filename, struct Status* st)
{
	(void)st;

#ifdef DEBUG
	DEBUG_PRINT("(OptionsReadFile, '%s')\n", filename);
	DictionaryIterate((struct Dictionary*)options, sPrintCallback, NULL);
#else
	(void)options;
	(void)filename;
	(void)st;
#endif

	return 1;
}


/*-----------------------------

 sOptionsRegister()
-----------------------------*/
static struct Option* sOptionsRegister(struct Options* options, const char* key, enum Type type, struct Status* st)
{
	struct DictionaryItem* item = NULL;
	struct Option* option = NULL;

	StatusSet(st, "sOptionsRegister", STATUS_SUCCESS, NULL);

	for (const char* c = key; *c != '\0'; c++)
	{
		if (isalnum((int)*c) == 0 // If not
		    && *c != '_')
		{
			StatusSet(st, "sOptionsRegister", STATUS_INVALID_ARGUMENT, "Only alphanumeric options names allowed");
			return NULL;
		}
	}

	if ((item = DictionaryAdd((struct Dictionary*)options, key, NULL, sizeof(struct Option))) == NULL)
	{
		StatusSet(st, "sOptionsRegister", STATUS_MEMORY_ERROR, NULL);
		return NULL;
	}

	option = item->data;

	memset(option, 0, sizeof(struct Option));
	option->type = type;

	return item->data;
}


/*-----------------------------

 sOptionsRetrieve()
-----------------------------*/
static inline const char* sTypeName(enum Type type)
{
	switch (type)
	{
	case TYPE_INT: return "integer";
	case TYPE_FLOAT: return "decimal";
	case TYPE_BOOL: return "boolean";
	case TYPE_STRING: return "string";
	}

	return NULL;
}

static struct Option* sOptionsRetrieve(const struct Options* options, const char* key, enum Type type,
                                       struct Status* st)
{
	struct DictionaryItem* item = NULL;
	struct Option* option = NULL;

	StatusSet(st, "sOptionsRetrieve", STATUS_SUCCESS, NULL);

	if ((item = DictionaryGet((struct Dictionary*)options, key)) == NULL)
	{
		StatusSet(st, "sOptionsRetrieve", STATUS_ERROR, "Option '%s' not registered", key);
		return NULL;
	}

	option = item->data;

	if (option->type != type)
	{
		StatusSet(st, "sOptionsRetrieve", STATUS_ERROR, "Option '%s' has an %s value (%s requested)", key,
		          sTypeName(option->type), sTypeName(type));
		return NULL;
	}

	return option;
}


/*-----------------------------

 lol no generics
-----------------------------*/
EXPORT int OptionsRegisterInt(struct Options* options, const char* key, int default_value, int min, int max,
                              struct Status* st)
{
	struct Option* option = sOptionsRegister(options, key, TYPE_INT, st);
	if (option != NULL)
	{
		option->value.i = ClampInt(default_value, min, max);
		option->min.i = min;
		option->max.i = max;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRegisterBool(struct Options* options, const char* key, bool default_value, struct Status* st)
{
	struct Option* option = sOptionsRegister(options, key, TYPE_BOOL, st);
	if (option != NULL)
	{
		option->value.b = default_value;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRegisterFloat(struct Options* options, const char* key, float default_value, float min, float max,
                                struct Status* st)
{
	struct Option* option = sOptionsRegister(options, key, TYPE_FLOAT, st);
	if (option != NULL)
	{
		option->value.f = ClampFloat(default_value, min, max);
		option->min.f = min;
		option->max.f = max;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRegisterString(struct Options* options, const char* key, const char* default_value,
                                 const char* possible_values, struct Status* st)
{
	(void)possible_values;

	struct Option* option = sOptionsRegister(options, key, TYPE_STRING, st);
	if (option != NULL)
	{
		option->value.s = default_value;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRetrieveInt(const struct Options* options, const char* key, int* dest, struct Status* st)
{
	struct Option* option = sOptionsRetrieve(options, key, TYPE_INT, st);
	if (option != NULL)
	{
		*dest = option->value.i;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRetrieveBool(const struct Options* options, const char* key, bool* dest, struct Status* st)
{
	struct Option* option = sOptionsRetrieve(options, key, TYPE_BOOL, st);
	if (option != NULL)
	{
		*dest = option->value.b;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRetrieveFloat(const struct Options* options, const char* key, float* dest, struct Status* st)
{
	struct Option* option = sOptionsRetrieve(options, key, TYPE_FLOAT, st);
	if (option != NULL)
	{
		*dest = option->value.f;
		return 0;
	}

	return 1;
}

EXPORT int OptionsRetrieveString(const struct Options* options, const char* key, const char** dest, struct Status* st)
{
	struct Option* option = sOptionsRetrieve(options, key, TYPE_STRING, st);
	if (option != NULL)
	{
		*dest = option->value.s;
		return 0;
	}

	return 1;
}
