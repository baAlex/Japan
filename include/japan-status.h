/*-----------------------------

 [japan-status.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_STATUS_H
#define JAPAN_STATUS_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stdarg.h>

#define JA_STATUS_FNAME_LEN 32
#define JA_STATUS_EXPL_LEN 224

enum jaStatusCode
{
	JA_STATUS_SUCCESS = 0,
	JA_STATUS_ERROR,

	JA_STATUS_FS_ERROR,
	JA_STATUS_IO_ERROR,
	JA_STATUS_UNEXPECTED_EOF,
	JA_STATUS_UNEXPECTED_DATA,
	JA_STATUS_UNKNOWN_FILE_FORMAT,

	JA_STATUS_UNSUPPORTED_FEATURE,
	JA_STATUS_OBSOLETE_FEATURE,

	JA_STATUS_UNKNOWN_DATA_FORMAT,
	JA_STATUS_INVALID_ARGUMENT,

	JA_STATUS_MEMORY_ERROR,

	JA_STATUS_EXPECTED_KEY_TOKEN,
	JA_STATUS_EXPECTED_EQUAL_TOKEN,
	JA_STATUS_STATEMENT_OPEN,
	JA_STATUS_NO_ASSIGNMENT,
	JA_STATUS_INTEGER_CAST_ERROR,
	JA_STATUS_DECIMAL_CAST_ERROR,

	JA_STATUS_ASCII_ERROR,
	JA_STATUS_UTF8_ERROR
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
