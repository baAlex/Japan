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
#include "japan-dictionary.h"
#include "japan-options.h"
#include "japan-utilities.h"

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
	struct jaDictionary* possible_values;
};


#ifdef JA_DEBUG
static void sPrintCallback(struct jaDictionaryItem* item, void* data)
{
	(void)data;
	struct Option* option = (struct Option*)item->data;

	switch (option->type)
	{
	case TYPE_INT:
		JA_DEBUG_PRINT(" - %s = %i [i%s]\n", item->key, option->value.i,
		               (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_FLOAT:
		JA_DEBUG_PRINT(" - %s = %f [f%s]\n", item->key, option->value.f,
		               (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_BOOL:
		JA_DEBUG_PRINT(" - %s = %s [b%s]\n", item->key, (option->value.b == true) ? "true" : "false",
		               (option->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_STRING:
		JA_DEBUG_PRINT(" - %s = \"%s\" [s%s]\n", item->key, option->value.s,
		               (option->set_by == SET_DEFAULT) ? "" : ", (*)");
	}
}
#endif


/*-----------------------------

 jaOptionsCreate()
-----------------------------*/
inline struct jaOptions* jaOptionsCreate()
{
	return (struct jaOptions*)jaDictionaryCreate(NULL);
}


/*-----------------------------

 jaOptionsDelete()
-----------------------------*/
inline void jaOptionsDelete(struct jaOptions* options)
{
	jaDictionaryDelete((struct jaDictionary*)options);
}


/*-----------------------------

 jaOptionsReadArguments()
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

	*dest = jaClampFloat(value, min, max);
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

	*dest = jaClampInt((int)value, min, max);
	return 0;
}


void jaOptionsReadArguments(struct jaOptions* options, int argc, const char* argv[])
{
	struct jaDictionaryItem* item = NULL;
	struct Option* option = NULL;
	union Value old_value;

	for (int i = 1; i < argc; i++)
	{
		// Check key
		if (argv[i][0] != '-')
		{
			JA_DEBUG_PRINT("[Warning] Unexpected token '%s'\n", (argv[i]));
			continue;
		}

		if ((item = jaDictionaryGet((struct jaDictionary*)options, (argv[i] + 1))) == NULL)
		{
			JA_DEBUG_PRINT("[Warning] Unknown option '%s'\n", (argv[i] + 1));
			continue;
		}

		// Check value
		option = item->data;
		memcpy(&old_value, &option->value, sizeof(union Value));

		if ((i + 1) == argc)
		{
			JA_DEBUG_PRINT("[Warning] No value for option '%s'\n", (argv[i] + 1));

			i++; // Important!
			continue;
		}

		switch (option->type)
		{
		case TYPE_INT:
			if (sStoreInt(argv[i + 1], &option->value.i, option->min.i, option->max.i) != 0)
				JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a integer value as '%s' requires\n",
				               argv[i + 1], (argv[i] + 1));
			break;
		case TYPE_FLOAT:
			if (sStoreFloat(argv[i + 1], &option->value.f, option->min.f, option->max.f) != 0)
				JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a decimal value as '%s' requires\n",
				               argv[i + 1], (argv[i] + 1));
			break;
		case TYPE_BOOL:
			if (sStoreBool(argv[i + 1], &option->value.b) != 0)
				JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a boolean value as '%s' requires\n",
				               argv[i + 1], (argv[i] + 1));
			break;
		case TYPE_STRING: sStoreString(argv[i + 1], &option->value.s);
		}

		if (memcmp(&old_value, &option->value, sizeof(union Value)) != 0) // Something change?
			option->set_by = SET_BY_ARGUMENTS;

		i++; // Important!
	}

#ifdef JA_DEBUG
	JA_DEBUG_PRINT("(jaOptionsReadArguments)\n");
	jaDictionaryIterate((struct jaDictionary*)options, sPrintCallback, NULL);
#endif
}


/*-----------------------------

 jaOptionsReadFile()
-----------------------------*/
int jaOptionsReadFile(struct jaOptions* options, const char* filename, struct jaStatus* st)
{
	(void)st;

#ifdef JA_DEBUG
	JA_DEBUG_PRINT("(jaOptionsReadFile, '%s')\n", filename);
	jaDictionaryIterate((struct jaDictionary*)options, sPrintCallback, NULL);
#else
	(void)options;
	(void)filename;
	(void)st;
#endif

	return 1;
}


/*-----------------------------

 sRegister()
-----------------------------*/
static struct Option* sRegister(struct jaOptions* options, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct Option* option = NULL;

	jaStatusSet(st, "sRegister", STATUS_SUCCESS, NULL);

	for (const char* c = key; *c != '\0'; c++)
	{
		if (isalnum((int)*c) == 0 // If not
		    && *c != '_')
		{
			jaStatusSet(st, "sRegister", STATUS_INVALID_ARGUMENT, "Only alphanumeric options names allowed");
			return NULL;
		}
	}

	if ((item = jaDictionaryAdd((struct jaDictionary*)options, key, NULL, sizeof(struct Option))) == NULL)
	{
		jaStatusSet(st, "sRegister", STATUS_MEMORY_ERROR, NULL);
		return NULL;
	}

	option = item->data;

	memset(option, 0, sizeof(struct Option));
	option->type = type;

	return item->data;
}


/*-----------------------------

 sRetrieve()
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

static struct Option* sRetrieve(const struct jaOptions* options, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct Option* option = NULL;

	jaStatusSet(st, "sRetrieve", STATUS_SUCCESS, NULL);

	if ((item = jaDictionaryGet((struct jaDictionary*)options, key)) == NULL)
	{
		jaStatusSet(st, "sRetrieve", STATUS_ERROR, "Option '%s' not registered", key);
		return NULL;
	}

	option = item->data;

	if (option->type != type)
	{
		jaStatusSet(st, "sRetrieve", STATUS_ERROR, "Option '%s' has an %s value (%s requested)", key,
		            sTypeName(option->type), sTypeName(type));
		return NULL;
	}

	return option;
}


/*-----------------------------

 "Generics"
-----------------------------*/
int jaOptionsRegisterInt(struct jaOptions* options, const char* key, int default_value, int min, int max,
                         struct jaStatus* st)
{
	struct Option* option = sRegister(options, key, TYPE_INT, st);
	if (option != NULL)
	{
		option->value.i = jaClampInt(default_value, min, max);
		option->min.i = min;
		option->max.i = max;
		return 0;
	}

	return 1;
}


int jaOptionsRegisterBool(struct jaOptions* options, const char* key, bool default_value, struct jaStatus* st)
{
	struct Option* option = sRegister(options, key, TYPE_BOOL, st);
	if (option != NULL)
	{
		option->value.b = default_value;
		return 0;
	}

	return 1;
}


int jaOptionsRegisterFloat(struct jaOptions* options, const char* key, float default_value, float min, float max,
                           struct jaStatus* st)
{
	struct Option* option = sRegister(options, key, TYPE_FLOAT, st);
	if (option != NULL)
	{
		option->value.f = jaClampFloat(default_value, min, max);
		option->min.f = min;
		option->max.f = max;
		return 0;
	}

	return 1;
}


int jaOptionsRegisterString(struct jaOptions* options, const char* key, const char* default_value,
                            const char* possible_values, struct jaStatus* st)
{
	(void)possible_values;

	struct Option* option = sRegister(options, key, TYPE_STRING, st);
	if (option != NULL)
	{
		option->value.s = default_value;
		return 0;
	}

	return 1;
}


int jaOptionsRetrieveInt(const struct jaOptions* options, const char* key, int* dest, struct jaStatus* st)
{
	struct Option* option = sRetrieve(options, key, TYPE_INT, st);
	if (option != NULL)
	{
		*dest = option->value.i;
		return 0;
	}

	return 1;
}


int jaOptionsRetrieveBool(const struct jaOptions* options, const char* key, bool* dest, struct jaStatus* st)
{
	struct Option* option = sRetrieve(options, key, TYPE_BOOL, st);
	if (option != NULL)
	{
		*dest = option->value.b;
		return 0;
	}

	return 1;
}


int jaOptionsRetrieveFloat(const struct jaOptions* options, const char* key, float* dest, struct jaStatus* st)
{
	struct Option* option = sRetrieve(options, key, TYPE_FLOAT, st);
	if (option != NULL)
	{
		*dest = option->value.f;
		return 0;
	}

	return 1;
}


int jaOptionsRetrieveString(const struct jaOptions* options, const char* key, const char** dest, struct jaStatus* st)
{
	struct Option* option = sRetrieve(options, key, TYPE_STRING, st);
	if (option != NULL)
	{
		*dest = option->value.s;
		return 0;
	}

	return 1;
}
