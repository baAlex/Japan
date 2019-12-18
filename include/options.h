/*-----------------------------

 [options.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
#endif

#ifndef OPTIONS_H
#define OPTIONS_H

	#include <stdbool.h>
	#include "status.h"

	struct Options;

	JAPAN_API struct Options* OptionsCreate();
	JAPAN_API void OptionsDelete(struct Options*);

	JAPAN_API void OptionsReadArguments(struct Options*, int argc, const char* argv[]);
	JAPAN_API int OptionsReadFile(struct Options*, const char* filename, struct Status*);

	JAPAN_API int OptionsRegisterInt(struct Options*, const char* name, int default_value, int min, int max, struct Status*);
	JAPAN_API int OptionsRegisterBool(struct Options*, const char* name, bool default_value, struct Status*);
	JAPAN_API int OptionsRegisterFloat(struct Options*, const char* name, float default_value, float min, float max, struct Status*);
	JAPAN_API int OptionsRegisterString(struct Options*, const char* name, const char* default_value, const char* possible_values, struct Status*);

	JAPAN_API int OptionsRetrieveInt(const struct Options*, const char* name, int* dest, struct Status*);
	JAPAN_API int OptionsRetrieveBool(const struct Options*, const char* name, bool* dest, struct Status*);
	JAPAN_API int OptionsRetrieveFloat(const struct Options*, const char* name, float* dest, struct Status*);
	JAPAN_API int OptionsRetrieveString(const struct Options*, const char* name, const char** dest, struct Status*);

	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define OptionsRetrieve(options, name, dest, st) _Generic((dest), \
		int*: OptionsRetrieveInt, \
		bool*: OptionsRetrieveBool, \
		float*: OptionsRetrieveFloat, \
		char*: OptionsRetrieveString, \
		default: OptionsRetrieveInt \
	)(options, name, dest, st)

	#endif

#endif
