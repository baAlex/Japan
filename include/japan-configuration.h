/*-----------------------------

 [japan-configuration.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_CONFIGURATION_H
#define JAPAN_CONFIGURATION_H

#ifndef JA_WIP
#warning "LibJapan here!, be caution, this module is unstable"
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

#include <stdio.h>

#include "japan-status.h"
#include "japan-string.h"

enum jaArgumentsFlags
{
	JA_SKIP_FIRST = 0,
	JA_PARSE_FIRST
};

struct jaConfiguration;
struct jaCvar;

JA_EXPORT struct jaConfiguration* jaConfigurationCreate();
JA_EXPORT void jaConfigurationDelete(struct jaConfiguration*);

JA_EXPORT void jaConfigurationArguments(struct jaConfiguration* config, enum jaEncode, int argc, const char* argv[]);
JA_EXPORT void jaConfigurationArgumentsEx(struct jaConfiguration* config, enum jaEncode, enum jaArgumentsFlags,
                                          void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*),
                                          int argc, const char* argv[]);

JA_EXPORT struct jaCvar* jaCvarCreateInt(struct jaConfiguration*, const char* name, int default_value, int min, int max,
                                         struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateFloat(struct jaConfiguration*, const char* name, float default_value, float min,
                                           float max, struct jaStatus*);
JA_EXPORT struct jaCvar* jaCvarCreateString(struct jaConfiguration*, const char* name, const char* default_value,
                                            const char* allowed_values[], const char* prohibited_values[],
                                            struct jaStatus*);

JA_EXPORT struct jaCvar* jaCvarGet(const struct jaConfiguration*, const char* name);
JA_EXPORT void jaCvarDelete(struct jaCvar* cvar);

JA_EXPORT int jaCvarGetValueInt(const struct jaCvar*, int* dest, struct jaStatus*);
JA_EXPORT int jaCvarGetValueFloat(const struct jaCvar*, float* dest, struct jaStatus*);
JA_EXPORT int jaCvarGetValueString(const struct jaCvar*, const char** dest, struct jaStatus*);

#endif
