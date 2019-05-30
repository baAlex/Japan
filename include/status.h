/*-----------------------------

 [status.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef STATUS_H
#define STATUS_H

	#include <stdarg.h>

	#define STATUS_FUNCTION_NAME_LENGTH 32
	#define STATUS_EXPLANATION_LENGTH 224

	enum StatusCode
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
	};

	struct Status
	{
		enum StatusCode code;
		char function_name[STATUS_FUNCTION_NAME_LENGTH];
		char explanation[STATUS_EXPLANATION_LENGTH];
	};

	void StatusSet(struct Status* st, const char* function_name, enum StatusCode code, const char* explanation_fmt, ...);
	int StatusPrint(struct Status st);

#endif
