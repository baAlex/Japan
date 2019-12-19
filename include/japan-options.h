/*-----------------------------

 [japan-options.h]
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

#include <stdbool.h>
#include "japan-status.h"

struct jaOptions;

JA_EXPORT struct jaOptions* jaOptionsCreate();
JA_EXPORT void jaOptionsDelete(struct jaOptions*);

JA_EXPORT void jaOptionsReadArguments(struct jaOptions*, int argc, const char* argv[]);
JA_EXPORT int jaOptionsReadFile(struct jaOptions*, const char* filename, struct jaStatus*);

JA_EXPORT int jaOptionsRegisterInt(struct jaOptions*, const char* name, int default_value, int min, int max,
                                   struct jaStatus*);
JA_EXPORT int jaOptionsRegisterBool(struct jaOptions*, const char* name, bool default_value, struct jaStatus*);
JA_EXPORT int jaOptionsRegisterFloat(struct jaOptions*, const char* name, float default_value, float min, float max,
                                     struct jaStatus*);
JA_EXPORT int jaOptionsRegisterString(struct jaOptions*, const char* name, const char* default_value,
                                      const char* possible_values, struct jaStatus*);

JA_EXPORT int jaOptionsRetrieveInt(const struct jaOptions*, const char* name, int* dest, struct jaStatus*);
JA_EXPORT int jaOptionsRetrieveBool(const struct jaOptions*, const char* name, bool* dest, struct jaStatus*);
JA_EXPORT int jaOptionsRetrieveFloat(const struct jaOptions*, const char* name, float* dest, struct jaStatus*);
JA_EXPORT int jaOptionsRetrieveString(const struct jaOptions*, const char* name, const char** dest, struct jaStatus*);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaOptionsRetrieve(options, name, dest, st)                                                                     \
	_Generic((dest), \
		int*: jaOptionsRetrieveInt, \
		bool*: jaOptionsRetrieveBool, \
		float*: jaOptionsRetrieveFloat, \
		char*: jaOptionsRetrieveString, \
		default: jaOptionsRetrieveInt \
	)(options, name, dest, st)

#endif

#endif
