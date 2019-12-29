/*-----------------------------

 [japan-config.h]
 - Alexander Brandt 2019
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


#ifndef JA_OPTIONS_H
#define JA_OPTIONS_H

#include <stdio.h>
#include <limits.h>
#include "japan-status.h"

enum jaConfigArgumentsFlags
{
	ARGUMENTS_DEFAULT = 0,
	ARGUMENTS_INCLUDE_FIRST_ONE = 1,
	ARGUMENTS_TOKENIZE_ONLY = 2,
	ARGUMENTS_NO_ASSIGNMENT = 4
};

enum jaConfigFileFlags
{
	FILE_DEFAULT = 0,
	FILE_TOKENIZE_ONLY = 1,
	FILE_NO_ASSIGNMENT = 2,
};

struct jaConfig;

JA_EXPORT struct jaConfig* jaConfigCreate();
JA_EXPORT void jaConfigDelete(struct jaConfig*);

JA_EXPORT int jaConfigReadArguments(struct jaConfig*, int argc, const char* argv[], struct jaStatus*);
JA_EXPORT int jaConfigReadArgumentsEx(struct jaConfig*, int argc, const char* argv[], enum jaConfigArgumentsFlags, struct jaStatus*);

JA_EXPORT int jaConfigReadFile(struct jaConfig*, const char* filename, struct jaStatus*);
JA_EXPORT int jaConfigReadFileEx(struct jaConfig*, FILE* fp, enum jaConfigFileFlags, struct jaStatus*);

JA_EXPORT int jaConfigRegisterInt(struct jaConfig*, const char* name, int default_value, int min, int max,
                                  struct jaStatus*);
JA_EXPORT int jaConfigRegisterFloat(struct jaConfig*, const char* name, float default_value, float min, float max,
                                    struct jaStatus*);
JA_EXPORT int jaConfigRegisterString(struct jaConfig*, const char* name, const char* default_value,
                                     const char* allowed_values, const char* prohibited_values, struct jaStatus*);

JA_EXPORT int jaConfigRetrieveInt(const struct jaConfig*, const char* name, int* dest, struct jaStatus*);
JA_EXPORT int jaConfigRetrieveFloat(const struct jaConfig*, const char* name, float* dest, struct jaStatus*);
JA_EXPORT int jaConfigRetrieveString(const struct jaConfig*, const char* name, const char** dest, struct jaStatus*);

#define jaConfigRegister(config, name, default_value, a, b, st)\
	_Generic((default_value),\
		int : jaConfigRegisterInt,\
		float : jaConfigRegisterFloat,\
		char* : jaConfigRegisterString,\
		const char* : jaConfigRegisterString,\
		default : jaConfigRegisterInt\
	)(config, name, default_value, a, b, st)

#define jaConfigRetrieve(config, name, dest, st)\
	_Generic((dest),\
		int* : jaConfigRetrieveInt,\
		float* : jaConfigRetrieveFloat,\
		char** : jaConfigRetrieveString,\
		const char** : jaConfigRetrieveString,\
		default : jaConfigRetrieveInt\
	)(config, name, dest, st)

#endif
