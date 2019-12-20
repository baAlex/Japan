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

 [config.c]
 - Alexander Brandt 2019
-----------------------------*/

#include "private.h"


#ifdef JA_DEBUG
void PrintCallback(struct jaDictionaryItem* item, void* data)
{
	(void)data;
	struct Cvar* cvar = (struct Cvar*)item->data;

	switch (cvar->type)
	{
	case TYPE_INT:
		JA_DEBUG_PRINT(" - %s = %i [i%s]\n", item->key, cvar->value.i, (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_FLOAT:
		JA_DEBUG_PRINT(" - %s = %f [f%s]\n", item->key, cvar->value.f, (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
		break;
	case TYPE_STRING:
		JA_DEBUG_PRINT(" - %s = '%s' [s%s]\n", item->key, cvar->value.s, (cvar->set_by == SET_DEFAULT) ? "" : ", (*)");
	}
}
#endif


/*-----------------------------

 StoreFloat()
-----------------------------*/
inline int StoreFloat(float* dest, const char* org, float min, float max)
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


/*-----------------------------

 StoreInt()
-----------------------------*/
inline int StoreInt(int* dest, const char* org, int min, int max)
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

				if (StoreFloat(&temp, org, (float)min, (float)max) != 0)
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


/*-----------------------------

 StoreString()
-----------------------------*/
inline void StoreString(const char** dest, const char* org)
{
	// TODO
	*dest = org;
}


/*-----------------------------

 jaConfigCreate()
-----------------------------*/
inline struct jaConfig* jaConfigCreate()
{
	return (struct jaConfig*)jaDictionaryCreate(NULL);
}


/*-----------------------------

 jaConfigDelete()
-----------------------------*/
inline void jaConfigDelete(struct jaConfig* config)
{
	jaDictionaryDelete((struct jaDictionary*)config);
}


/*-----------------------------

 sRegister()
-----------------------------*/
static struct Cvar* sRegister(struct jaConfig* config, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct Cvar* cvar = NULL;

	jaStatusSet(st, "sRegister", STATUS_SUCCESS, NULL);

	for (const char* c = key; *c != '\0'; c++)
	{
		if (isalnum((int)*c) == 0 // If not
		    && *c != '_')
		{
			jaStatusSet(st, "sRegister", STATUS_INVALID_ARGUMENT, "Only alphanumeric keys allowed");
			return NULL;
		}
	}

	if ((item = jaDictionaryAdd((struct jaDictionary*)config, key, NULL, sizeof(struct Cvar))) == NULL)
	{
		jaStatusSet(st, "sRegister", STATUS_MEMORY_ERROR, NULL);
		return NULL;
	}

	cvar = item->data;

	memset(cvar, 0, sizeof(struct Cvar));
	cvar->type = type;

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
	case TYPE_STRING: return "string";
	}

	return NULL;
}

static struct Cvar* sRetrieve(const struct jaConfig* config, const char* key, enum Type type, struct jaStatus* st)
{
	struct jaDictionaryItem* item = NULL;
	struct Cvar* cvar = NULL;

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
int jaConfigRegisterInt(struct jaConfig* config, const char* key, int default_value, int min, int max,
                        struct jaStatus* st)
{
	struct Cvar* cvar = sRegister(config, key, TYPE_INT, st);
	if (cvar != NULL)
	{
		cvar->value.i = jaClampInt(default_value, min, max);
		cvar->min.i = min;
		cvar->max.i = max;
		return 0;
	}

	return 1;
}

int jaConfigRegisterFloat(struct jaConfig* config, const char* key, float default_value, float min, float max,
                          struct jaStatus* st)
{
	struct Cvar* cvar = sRegister(config, key, TYPE_FLOAT, st);
	if (cvar != NULL)
	{
		cvar->value.f = jaClampFloat(default_value, min, max);
		cvar->min.f = min;
		cvar->max.f = max;
		return 0;
	}

	return 1;
}

int jaConfigRegisterString(struct jaConfig* config, const char* key, const char* default_value,
                           const char* allowed_values, const char* prohibited_values, struct jaStatus* st)
{
	(void)allowed_values;
	(void)prohibited_values;

	struct Cvar* cvar = sRegister(config, key, TYPE_STRING, st);
	if (cvar != NULL)
	{
		cvar->value.s = default_value;
		return 0;
	}

	return 1;
}

int jaConfigRetrieveInt(const struct jaConfig* config, const char* key, int* dest, struct jaStatus* st)
{
	struct Cvar* cvar = sRetrieve(config, key, TYPE_INT, st);
	if (cvar != NULL)
	{
		*dest = cvar->value.i;
		return 0;
	}

	return 1;
}

int jaConfigRetrieveFloat(const struct jaConfig* config, const char* key, float* dest, struct jaStatus* st)
{
	struct Cvar* cvar = sRetrieve(config, key, TYPE_FLOAT, st);
	if (cvar != NULL)
	{
		*dest = cvar->value.f;
		return 0;
	}

	return 1;
}

int jaConfigRetrieveString(const struct jaConfig* config, const char* key, const char** dest, struct jaStatus* st)
{
	struct Cvar* cvar = sRetrieve(config, key, TYPE_STRING, st);
	if (cvar != NULL)
	{
		*dest = cvar->value.s;
		return 0;
	}

	return 1;
}
