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
		strcpy(b->data, string);
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

#ifdef JA_DEBUG
void PrintCallback(struct jaDictionaryItem* item, void* data)
{
	(void)data;
	struct jaCvar* cvar = (struct jaCvar*)item->data;

	switch (cvar->type)
	{
	case TYPE_INT:
		JA_DEBUG_PRINT(" - %s = %i [i%s]\n", item->key, cvar->value.i, (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_FLOAT:
		JA_DEBUG_PRINT(" - %s = %f [f%s]\n", item->key, cvar->value.f, (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_STRING:
		JA_DEBUG_PRINT(" - %s = '%s' [s%s]\n", item->key, sBufferGetString(&cvar->value.s),
		               (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
	}
}
#endif


/*-----------------------------

 ValidateKey()
-----------------------------*/
int ValidateKey(const char* string, size_t len)
{
	// Only ASCII letters, underscores, dots, and
	// numbers are valid characters. The first
	// character should be a letter. No consecutive
	// dots.

	if ((string[0] < 0x41 && string[0] > 0x5A) && // A-Z
	    (string[0] < 0x61 && string[0] > 0x7A))   // a-z
		return 1;

	for (size_t i = 1; i < len; i++)
	{
		if ((string[i] < 0x41 && string[i] > 0x5A) && // A-Z
		    (string[i] < 0x61 && string[i] > 0x7A) && // a-z
		    (string[i] < 0x30 && string[i] > 0x39) && // 0-9
		    (string[i] != 0x5F) &&                    // Underscore
		    (string[i] != 0x2E))                      // Full stop (dot)
			return 1;

		if (string[i] == 0x2E && string[i - 1] == 0x2E) // Consecutive dots
			return 1;
	}

	return 0;
}


/*-----------------------------

 Store()
-----------------------------*/
static int sStoreFloat(float* dest, const char* org, float min, float max, bool* changes)
{
	float old_value = *dest;
	char* end = NULL;
	float value = strtof(org, &end); // "Past the last character interpreted"

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

static int sStoreInt(int* dest, const char* org, int min, int max, bool* changes)
{
	int old_value = *dest;
	char* end = NULL;
	long value = strtol(org, &end, 0);

	if (*end != '\0')
		for (; *end != '\0'; end++)
		{
			if (isspace((int)*end) == 0)
			{
				// Try with float
				float temp = 0.0f;

				if (sStoreFloat(&temp, org, (float)min, (float)max, changes) != 0)
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

int Store(struct jaCvar* cvar, const char* token, enum SetBy by)
{
	bool changes = false;

	switch (cvar->type)
	{
	case TYPE_INT:
		if (sStoreInt(&cvar->value.i, token, cvar->min.i, cvar->max.i, &changes) != 0)
		{
			JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a integer value as '%s' requires\n", token,
			               cvar->item->key);
			return 1;
		}
		break;

	case TYPE_FLOAT:
		if (sStoreFloat(&cvar->value.f, token, cvar->min.f, cvar->max.f, &changes) != 0)
		{
			JA_DEBUG_PRINT("[Warning] Token '%s' can't be cast into a decimal value as '%s' requires\n", token,
			               cvar->item->key);
			return 1;
		}
		break;

	case TYPE_STRING:
		if (sStoreString(&cvar->value.s, token, &changes) != 0)
		{
			JA_DEBUG_PRINT("[Warning] Not enought memory to store value '%s' as '%s' requires\n", token,
			               cvar->item->key);
			return 1;
		}
	}

	if (changes == true)
		cvar->set_by = by;

	return 0;
}


/*-----------------------------

 jaConfigurationCreate()
-----------------------------*/
inline struct jaConfiguration* jaConfigurationCreate()
{
	return (struct jaConfiguration*)jaDictionaryCreate(NULL);
}


/*-----------------------------

 jaConfigurationDelete()
-----------------------------*/
static void sDeleteCallback(struct jaDictionaryItem* item)
{
	struct jaCvar* cvar = item->data;

	if (cvar->type == TYPE_STRING)
		jaBufferClean(&cvar->value.s);
}

inline void jaConfigurationDelete(struct jaConfiguration* config)
{
	jaDictionaryDelete((struct jaDictionary*)config);
}


/*-----------------------------

 sRegister()
-----------------------------*/
static struct jaCvar* sRegister(struct jaConfiguration* config, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct jaCvar* cvar = NULL;

	jaStatusSet(st, "sRegister", STATUS_SUCCESS, NULL);

	if (ValidateKey(key, strlen(key)) != 0)
	{
		jaStatusSet(st, "sRegister", STATUS_INVALID_ARGUMENT, "Invalid key");
		return NULL;
	}

	if ((item = jaDictionaryAdd((struct jaDictionary*)config, key, NULL, sizeof(struct jaCvar))) == NULL)
	{
		jaStatusSet(st, "sRegister", STATUS_MEMORY_ERROR, NULL);
		return NULL;
	}

	item->callback_delete = sDeleteCallback;
	cvar = item->data;

	memset(cvar, 0, sizeof(struct jaCvar));
	cvar->type = type;
	cvar->item = item;

	return cvar;
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
	case TYPE_STRING: return "string";
	}

	return NULL;
}

static struct jaCvar* sRetrieve(const struct jaConfiguration* config, const char* key, enum Type type,
                                struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct jaCvar* cvar = NULL;

	jaStatusSet(st, "sRetrieve", STATUS_SUCCESS, NULL);

	if ((item = jaDictionaryGet((struct jaDictionary*)config, key)) == NULL)
	{
		jaStatusSet(st, "sRetrieve", STATUS_ERROR, "Configuration '%s' not registered", key);
		return NULL;
	}

	cvar = item->data;

	if (cvar->type != type)
	{
		jaStatusSet(st, "sRetrieve", STATUS_ERROR, "Configuration '%s' has an %s value (%s requested)", key,
		            sTypeName(cvar->type), sTypeName(type));
		return NULL;
	}

	return cvar;
}


/*-----------------------------

 "Generics"
-----------------------------*/
struct jaCvar* jaCvarCreateInt(struct jaConfiguration* config, const char* key, int default_value, int min, int max,
                               struct jaStatus* st)
{
	struct jaCvar* cvar = sRegister(config, key, TYPE_INT, st);

	if (cvar != NULL)
	{
		cvar->value.i = jaClampInt(default_value, min, max);
		cvar->min.i = min;
		cvar->max.i = max;
		return 0;
	}

	return cvar;
}

struct jaCvar* jaCvarCreateFloat(struct jaConfiguration* config, const char* key, float default_value, float min,
                                 float max, struct jaStatus* st)
{
	struct jaCvar* cvar = sRegister(config, key, TYPE_FLOAT, st);

	if (cvar != NULL)
	{
		cvar->value.f = jaClampFloat(default_value, min, max);
		cvar->min.f = min;
		cvar->max.f = max;
		return 0;
	}

	return cvar;
}

struct jaCvar* jaCvarCreateString(struct jaConfiguration* config, const char* key, const char* default_value,
                                  const char* allowed_values, const char* prohibited_values, struct jaStatus* st)
{
	(void)allowed_values;
	(void)prohibited_values;

	struct jaCvar* cvar = sRegister(config, key, TYPE_STRING, st);

	if (cvar != NULL)
	{
		sBufferSaveString(&cvar->value.s, default_value);
		return 0;
	}

	return cvar;
}

int jaCvarRetrieveInt(const struct jaConfiguration* config, const char* key, int* dest, struct jaStatus* st)
{
	struct jaCvar* cvar = sRetrieve(config, key, TYPE_INT, st);

	if (cvar != NULL)
	{
		*dest = cvar->value.i;
		return 0;
	}

	return 1;
}

int jaCvarRetrieveFloat(const struct jaConfiguration* config, const char* key, float* dest, struct jaStatus* st)
{
	struct jaCvar* cvar = sRetrieve(config, key, TYPE_FLOAT, st);

	if (cvar != NULL)
	{
		*dest = cvar->value.f;
		return 0;
	}

	return 1;
}

int jaCvarRetrieveString(const struct jaConfiguration* config, const char* key, const char** dest, struct jaStatus* st)
{
	struct jaCvar* cvar = sRetrieve(config, key, TYPE_STRING, st);

	if (cvar != NULL)
	{
		*dest = sBufferGetString(&cvar->value.s);
		return 0;
	}

	return 1;
}
