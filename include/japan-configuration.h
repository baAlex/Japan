/*-----------------------------

 [japan-configuration.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#ifdef JA_EXPORT_SYMBOLS
#if defined(__clang__) || defined(__GNUC__)
#define JA_EXPORT __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#define JA_EXPORT __declspec(dllexport)
#endif
#else
#define JA_EXPORT // Whitespace
#endif


#ifndef JA_CONFIGURATION_H
#define JA_CONFIGURATION_H

#include "japan-status.h"
#include <limits.h>
#include <stdio.h>

enum jaArgumentsFlags
{
	ARGUMENTS_DEFAULT = 0,
	ARGUMENTS_INCLUDE_FIRST_ONE = 1,
	ARGUMENTS_TOKENIZE_ONLY = 2,
	ARGUMENTS_NO_ASSIGNMENT = 4
};

enum jaFileFlags
{
	FILE_DEFAULT = 0,
	FILE_TOKENIZE_ONLY = 1,
	FILE_NO_ASSIGNMENT = 2,
};

struct jaConfiguration;
struct jaCvar;


JA_EXPORT struct jaConfiguration* jaConfigurationCreate();
JA_EXPORT void jaConfigurationDelete(struct jaConfiguration*);

JA_EXPORT int jaConfigurationArguments(struct jaConfiguration*, int argc, const char* argv[], struct jaStatus*);
JA_EXPORT int jaConfigurationArgumentsEx(struct jaConfiguration*, int argc, const char* argv[], enum jaArgumentsFlags,
                                         struct jaStatus*);

JA_EXPORT int jaConfigurationFile(struct jaConfiguration*, const char* filename, struct jaStatus*);
JA_EXPORT int jaConfigurationFileEx(struct jaConfiguration*, FILE* fp, enum jaFileFlags, struct jaStatus*);


JA_EXPORT struct jaCvar* jaCvarCreateInt(struct jaConfiguration*, const char* name, int default_value, int min, int max,
                                         struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateFloat(struct jaConfiguration*, const char* name, float default_value, float min,
                                           float max, struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateString(struct jaConfiguration*, const char* name, const char* default_value,
                                            const char* allowed_values, const char* prohibited_values,
                                            struct jaStatus*);

JA_EXPORT struct jaCvar* jaCvarGet(const struct jaConfiguration*, const char* name);
JA_EXPORT void jaCvarDelete(struct jaCvar* cvar);

JA_EXPORT int jaCvarRetrieveInt(const struct jaConfiguration*, const char* name, int* dest, struct jaStatus*);
JA_EXPORT int jaCvarRetrieveFloat(const struct jaConfiguration*, const char* name, float* dest, struct jaStatus*);
JA_EXPORT int jaCvarRetrieveString(const struct jaConfiguration*, const char* name, const char** dest,
                                   struct jaStatus*);

#define jaCvarCreate(config, name, default_value, a, b, st)\
	_Generic((default_value),\
		int : jaCvarCreateInt,\
		float : jaCvarCreateFloat,\
		char* : jaCvarCreateString,\
		const char* : jaCvarCreateString,\
		default : jaCvarCreateInt\
	)(config, name, default_value, a, b, st)

#define jaCvarRetrieve(config, name, dest, st)\
	_Generic((dest),\
		int* : jaCvarRetrieveInt,\
		float* : jaCvarRetrieveFloat,\
		char** : jaCvarRetrieveString,\
		const char** : jaCvarRetrieveString,\
		default : jaCvarRetrieveInt\
	)(config, name, dest, st)

#endif
