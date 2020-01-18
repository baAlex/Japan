/*-----------------------------

 [japan-status.h]
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


#ifndef JA_STATUS_H
#define JA_STATUS_H

#include <stdarg.h>

#define JA_STATUS_FNAME_LEN 32
#define JA_STATUS_EXPL_LEN 224

enum jaStatusCode
{
	STATUS_SUCCESS = 0,
	STATUS_ERROR,

	STATUS_FS_ERROR,
	STATUS_IO_ERROR,
	STATUS_UNEXPECTED_EOF,
	STATUS_UNEXPECTED_DATA,
	STATUS_UNKNOWN_FILE_FORMAT,

	STATUS_UNSUPPORTED_FEATURE,
	STATUS_OBSOLETE_FEATURE,

	STATUS_UNKNOWN_DATA_FORMAT,
	STATUS_INVALID_ARGUMENT,

	STATUS_MEMORY_ERROR,

	STATUS_EXPECTED_KEY_TOKEN,
	STATUS_EXPECTED_EQUAL_TOKEN,
	STATUS_STATEMENT_OPEN,
	STATUS_NO_ASSIGNMENT
};

struct jaStatus
{
	enum jaStatusCode code;
	char function_name[JA_STATUS_FNAME_LEN];
	char explanation[JA_STATUS_EXPL_LEN];
};

JA_EXPORT void jaStatusSet(struct jaStatus* st, const char* function_name, enum jaStatusCode code,
                           const char* explanation_fmt, ...);
JA_EXPORT void jaStatusCopy(const struct jaStatus* org, struct jaStatus* dest);
JA_EXPORT int jaStatusPrint(const char* app_name, struct jaStatus st);

#endif
