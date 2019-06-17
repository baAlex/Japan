/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [status.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdio.h>
#include <string.h>

#include "status.h"

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


EXPORT void StatusSet(struct Status* st, const char* function_name, enum StatusCode code, const char* explanation_fmt, ...)
{
	va_list args;

	if (st == NULL)
		return;

	st->code = code;

	if (function_name != NULL)
		strncpy(st->function_name, function_name, STATUS_FUNCTION_NAME_LENGTH);

	if (explanation_fmt != NULL)
	{
		va_start(args, explanation_fmt);
		vsnprintf(st->explanation, STATUS_EXPLANATION_LENGTH, explanation_fmt, args); // NOLINT(clang-analyzer-valist.Uninitialized)
		va_end(args);
	}
}


EXPORT int StatusPrint(struct Status st)
{
	char* explanation = (st.explanation[0] != '\0') ? st.explanation : "";
	char* code_message = NULL;

	switch (st.code)
	{
	case STATUS_SUCCESS:
		code_message = "Success";
		break;
	case STATUS_ERROR:
		code_message = "Unknown error";
		break;
	case STATUS_FS_ERROR:
		code_message = "Filesystem error";
		break;
	case STATUS_IO_ERROR:
		code_message = "File Input/Output error";
		break;
	case STATUS_UNEXPECTED_EOF:
		code_message = "End of file prematurely reached";
		break;
	case STATUS_UNEXPECTED_DATA:
		code_message = "Unexpected data";
		break;
	case STATUS_UNKNOWN_FILE_FORMAT:
		code_message = "Unknown file format";
		break;
	case STATUS_UNSUPPORTED_FEATURE:
		code_message = "Unsupported feature";
		break;
	case STATUS_OBSOLETE_FEATURE:
		code_message = "Obsolete feature";
		break;
	case STATUS_UNKNOWN_DATA_FORMAT:
		code_message = "Unknown data format";
		break;
	case STATUS_INVALID_ARGUMENT:
		code_message = "Invalid argument";
		break;
	case STATUS_MEMORY_ERROR:
		code_message = "Memory error";
		break;
	default:
		code_message = "Unknown status";
	}

	return fprintf(stderr, "%s : %s%s%s.\n", st.function_name, code_message, (st.explanation[0] != '\0') ? ", " : "", explanation);
}
