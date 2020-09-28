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

 [configuration.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "private.h"


static inline int sBufferSaveString(struct jaBuffer* b, const char* string)
{
	if (jaBufferResize(b, strlen(string) + 1) != NULL)
	{
		strncpy(b->data, string, strlen(string) + 1);
		return 0;
	}

	return 1;
}


static inline const char* sBufferGetString(const struct jaBuffer* b)
{
	if (b->size != 0)
		return b->data;

	return NULL;
}


inline struct jaConfiguration* jaConfigurationCreate()
{
	return (struct jaConfiguration*)jaDictionaryCreate(NULL);
}


inline void jaConfigurationDelete(struct jaConfiguration* config)
{
	jaDictionaryDelete((struct jaDictionary*)config);
}


static void sCvarDeleteCallback(struct jaDictionaryItem* item)
{
	struct jaCvar* cvar = item->data;

	if (cvar->type == TYPE_STRING)
		jaBufferClean(&cvar->value.s);
}


static int sValidateKey(const char* string)
{
	size_t i = 0;
	int last_is_dot = 0;

	// Plain old printable-ASCII

	// Ensure that the first character is a letter
	if (!(string[0] >= 0x41 && string[0] <= 0x5A) && // A-Z
	    !(string[0] >= 0x61 && string[0] <= 0x7A))   // a-z
		return 1;

	// For the remain characters, letters, numbers, non consecutive
	// dots and underscores, allowed
	for (i = 0;; i++)
	{
		if (string[i] == 0x00) // NULL
			break;

		if (!(string[i] >= 0x41 && string[i] <= 0x5A) && // A-Z
		    !(string[i] >= 0x61 && string[i] <= 0x7A) && // a-z
		    !(string[i] >= 0x30 && string[i] <= 0x39) && // 0-9
		    !(string[i] == 0x5F) &&                      // Underscore
		    !(string[i] == 0x2E))                        // Full stop (dot)
			break;

		if (string[i] == 0x2E) // Full stop (dot)
		{
			if (last_is_dot == 1) // Consecutive dots
				break;

			last_is_dot = 1;
		}
		else
			last_is_dot = 0;
	}

	// The key can't end in a dot!
	return (string[i] == 0x00) ? last_is_dot : 1;
}


static struct jaCvar* sCvarCreate(struct jaConfiguration* config, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct jaCvar* cvar = NULL;

	jaStatusSet(st, "jaCvarCreate", JA_STATUS_SUCCESS, NULL);

	if (key == NULL || sValidateKey(key) != 0)
	{
		jaStatusSet(st, "jaCvarCreate", JA_STATUS_INVALID_ARGUMENT, "Invalid key");
		return NULL;
	}

	if ((item = jaDictionaryAdd((struct jaDictionary*)config, key, NULL, sizeof(struct jaCvar))) == NULL)
	{
		jaStatusSet(st, "jaCvarCreate", JA_STATUS_MEMORY_ERROR, NULL);
		return NULL;
	}

	item->callback_delete = sCvarDeleteCallback;
	cvar = item->data;

	memset(cvar, 0, sizeof(struct jaCvar));
	cvar->type = type;
	cvar->item = item;

	return cvar;
}


static int sStoreFloat(float* dest, const char* to_store, float min, float max, bool* changes)
{
	float old_value = *dest;
	char* end = NULL;
	float value = strtof(to_store, &end); // "Past the last character interpreted"

	if (*end != '\0')
		for (; *end != '\0'; end++)
		{
			if (isspace((int)*end) == 0) // If not
				return 1;
		}

	*dest = jaClampFloat(value, min, max);
	*changes = (old_value == *dest) ? false : true;
	return 0;
}


static int sStoreInt(int* dest, const char* to_store, int min, int max, bool* changes)
{
	int old_value = *dest;
	char* end = NULL;
	long value = strtol(to_store, &end, 0);

	if (*end != '\0')
		for (; *end != '\0'; end++)
		{
			if (isspace((int)*end) == 0)
			{
				// Try with float
				float temp = 0.0f;

				if (sStoreFloat(&temp, to_store, (float)min, (float)max, changes) != 0)
					return 1;

				value = lroundf(temp);
				break;
			}
		}

	if (value > INT_MAX || value < INT_MIN)
		return 1;

	*dest = jaClampInt((int)value, min, max);
	*changes = (old_value == *dest) ? false : true;
	return 0;
}


static int sStoreString(struct jaBuffer* b, const char* str, bool* changes)
{
	*changes = (strcmp(sBufferGetString(b), str) == 0) ? false : true;
	return sBufferSaveString(b, str);
}


enum jaStatusCode Store(struct jaCvar* cvar, const char* to_store, enum SetBy by)
{
	bool changes = false;

	if (cvar->type == TYPE_INT)
	{
		if (sStoreInt(&cvar->value.i, to_store, cvar->min.i, cvar->max.i, &changes) != 0)
			return JA_STATUS_INTEGER_CAST_ERROR;
	}
	else if (cvar->type == TYPE_FLOAT)
	{
		if (sStoreFloat(&cvar->value.f, to_store, cvar->min.f, cvar->max.f, &changes) != 0)
			return JA_STATUS_DECIMAL_CAST_ERROR;
	}
	else if (cvar->type == TYPE_STRING)
	{
		if (sStoreString(&cvar->value.s, to_store, &changes) != 0)
			return JA_STATUS_MEMORY_ERROR;
	}

	if (changes == true)
		cvar->set_by = by;

	return JA_STATUS_SUCCESS;
}


struct jaCvar* jaCvarCreateInt(struct jaConfiguration* config, const char* name, int default_value, int min, int max,
                               struct jaStatus* st)
{
	struct jaCvar* cvar = sCvarCreate(config, name, TYPE_INT, st);

	if (cvar != NULL)
	{
		cvar->value.i = jaClampInt(default_value, min, max);
		cvar->min.i = min;
		cvar->max.i = max;
	}

	return cvar;
}


struct jaCvar* jaCvarCreateFloat(struct jaConfiguration* config, const char* name, float default_value, float min,
                                 float max, struct jaStatus* st)
{
	struct jaCvar* cvar = sCvarCreate(config, name, TYPE_FLOAT, st);

	if (cvar != NULL)
	{
		cvar->value.f = jaClampFloat(default_value, min, max);
		cvar->min.f = min;
		cvar->max.f = max;
	}

	return cvar;
}


struct jaCvar* jaCvarCreateString(struct jaConfiguration* config, const char* name, const char* default_value,
                                  const char* allowed_values[], const char* prohibited_values[], struct jaStatus* st)
{
	(void)allowed_values;
	(void)prohibited_values;

	struct jaCvar* cvar = sCvarCreate(config, name, TYPE_STRING, st);

	if (cvar != NULL)
		sBufferSaveString(&cvar->value.s, default_value);

	return cvar;
}


inline struct jaCvar* jaCvarGet(const struct jaConfiguration* config, const char* key)
{
	struct jaDictionaryItem* item = jaDictionaryGet((struct jaDictionary*)config, key);
	return (item != NULL) ? (struct jaCvar*)item->data : NULL;
}


inline void jaCvarDelete(struct jaCvar* cvar)
{
	jaDictionaryRemove(cvar->item); // Calls sCvarDeleteCallback()
}


int jaCvarGetValueInt(const struct jaCvar* cvar, int* dest, struct jaStatus* st)
{
	jaStatusSet(st, "jaGetCvarValueInt", JA_STATUS_SUCCESS, NULL);

	if (cvar == NULL)
	{
		jaStatusSet(st, "jaGetCvarValueInt", JA_STATUS_INVALID_ARGUMENT, NULL);
		return 1;
	}

	if (cvar->type == TYPE_INT)
		*dest = cvar->value.i;
	else if (cvar->type == TYPE_FLOAT)
		*dest = (int)roundf(cvar->value.f);
	else
	{
		jaStatusSet(st, "jaGetCvarValueInt", JA_STATUS_ERROR, "Can't cast '%s' into a integer", cvar->item->key);
		return 1;
	}

	return 0;
}


int jaCvarGetValueFloat(const struct jaCvar* cvar, float* dest, struct jaStatus* st)
{
	jaStatusSet(st, "jaCvarGetValueFloat", JA_STATUS_SUCCESS, NULL);

	if (cvar == NULL)
	{
		jaStatusSet(st, "jaCvarGetValueFloat", JA_STATUS_INVALID_ARGUMENT, NULL);
		return 1;
	}

	if (cvar->type == TYPE_FLOAT)
		*dest = cvar->value.f;
	else if (cvar->type == TYPE_INT)
		*dest = (float)cvar->value.i;
	else
	{
		jaStatusSet(st, "jaCvarGetValueFloat", JA_STATUS_ERROR, "Can't cast '%s' into a float", cvar->item->key);
		return 1;
	}

	return 0;
}


int jaCvarGetValueString(const struct jaCvar* cvar, const char** dest, struct jaStatus* st)
{
	jaStatusSet(st, "jaCvarGetValueString", JA_STATUS_SUCCESS, NULL);

	if (cvar == NULL)
	{
		jaStatusSet(st, "jaCvarGetValueString", JA_STATUS_INVALID_ARGUMENT, NULL);
		return 1;
	}

	if (cvar->type != TYPE_STRING)
	{
		jaStatusSet(st, "jaCvarGetValueString", JA_STATUS_ERROR, "Can't cast '%s' into a string", cvar->item->key);
		return 1;
	}

	*dest = sBufferGetString(&cvar->value.s);
	return 0;
}
