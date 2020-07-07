/*-----------------------------

 [japan-configuration.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_CONFIGURATION_H
#define JAPAN_CONFIGURATION_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "japan-status.h"

enum jaArgumentsFlags
{
	JA_ARGUMENTS_DEFAULT = 0,
	JA_ARGUMENTS_INCLUDE_FIRST_ONE,
	JA_ARGUMENTS_TOKENIZE_ONLY
};

enum jaFileFlags
{
	JA_FILE_DEFAULT = 0,
	JA_FILE_TOKENIZE_ONLY
};

struct jaTokenDelimiter
{
	bool eof : 1;
	bool ws : 1;
	bool nl : 1;
	bool sc : 1;
};

struct jaConfiguration;
struct jaCvar;

JA_EXPORT struct jaConfiguration* jaConfigurationCreate();
JA_EXPORT void jaConfigurationDelete(struct jaConfiguration*);

JA_EXPORT void jaConfigurationArguments(struct jaConfiguration*, int argc, const char* argv[]);
JA_EXPORT int jaConfigurationFile(struct jaConfiguration*, const char* filename, struct jaStatus*);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaCvarCreate(config, name, default_value, a, b, st)\
	_Generic((default_value),\
		int : jaCvarCreateInt,\
		float : jaCvarCreateFloat,\
		char* : jaCvarCreateString,\
		const char* : jaCvarCreateString,\
		default : jaCvarCreateInt\
	)(config, name, default_value, a, b, st)

#define jaCvarValue(cvar, dest, st)\
	_Generic((dest),\
		int* : jaCvarValueInt,\
		float* : jaCvarValueFloat,\
		char** : jaCvarValueString,\
		const char** : jaCvarValueString,\
		default : jaCvarValueInt\
	)(cvar, dest, st)

#endif

JA_EXPORT struct jaCvar* jaCvarGet(const struct jaConfiguration*, const char* name);
JA_EXPORT void jaCvarDelete(struct jaCvar* cvar); // TODO

// ----

JA_EXPORT void jaConfigurationArgumentsEx(struct jaConfiguration*, int argc, const char* argv[], enum jaArgumentsFlags,
                                          void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*));

JA_EXPORT int jaConfigurationFileEx(struct jaConfiguration*, FILE* fp, enum jaFileFlags,
                                    void (*tokenizer_callback)(int, struct jaTokenDelimiter, const char*),
                                    void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*),
                                    struct jaStatus*);

JA_EXPORT struct jaCvar* jaCvarCreateInt(struct jaConfiguration*, const char* name, int default_value, int min, int max,
                                         struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateFloat(struct jaConfiguration*, const char* name, float default_value, float min,
                                           float max, struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateString(struct jaConfiguration*, const char* name, const char* default_value,
                                            const char* allowed_values, const char* prohibited_values,
                                            struct jaStatus*);

JA_EXPORT int jaCvarValueInt(const struct jaCvar*, int* dest, struct jaStatus*);
JA_EXPORT int jaCvarValueFloat(const struct jaCvar*, float* dest, struct jaStatus*);
JA_EXPORT int jaCvarValueString(const struct jaCvar*, const char** dest, struct jaStatus*);

#endif
