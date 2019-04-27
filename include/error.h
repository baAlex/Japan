/*-----------------------------

 [error.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef ERROR_H
#define ERROR_H

	#include <stdarg.h>

	#define ERROR_FUNCTION_NAME_LENGTH 32
	#define ERROR_EXPLANATION_LENGTH 224

	enum ErrorCode
	{
		NO_ERROR = 0,
		ERROR_UNKHOWN,
		ERROR_FS,
		ERROR_IO,
		ERROR_BROKEN,
		ERROR_UNSUPPORTED,
		ERROR_UNKNOWN_FORMAT,
		ERROR_OBSOLETE,
		ERROR_ARGUMENT,
	};

	struct Error
	{
		enum ErrorCode code;
		char function_name[ERROR_FUNCTION_NAME_LENGTH];
		char explanation[ERROR_EXPLANATION_LENGTH];
	};

	void ErrorSet(struct Error* e, enum ErrorCode code, const char* function_name, const char* explanation_fmt, ...);
	int ErrorPrint(struct Error e);

#endif
