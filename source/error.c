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

 [error.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdio.h>
#include <string.h>

#include "error.h"


void ErrorSet(struct Error* e, enum ErrorCode code, const char* function_name, const char* explanation_fmt, ...)
{
	va_list args;

	e->code = code;

	if (function_name != NULL)
		strncpy(e->function_name, function_name, ERROR_FUNCTION_NAME_LENGTH);

	if (explanation_fmt != NULL)
	{
		va_start(args, explanation_fmt);
		vsnprintf(e->explanation, ERROR_EXPLANATION_LENGTH, explanation_fmt, args); // NOLINT(clang-analyzer-valist.Uninitialized)
		va_end(args);
	}
}


int ErrorPrint(struct Error e)
{
	char* explanation = (e.explanation[0] != '\0') ? e.explanation : "";
	char* code_message = NULL;

	switch (e.code)
	{
	case NO_ERROR:
		return 0;
	case ERROR_FS:
		code_message = "Filesystem error";
		break;
	case ERROR_IO:
		code_message = "Input/Output error";
		break;
	case ERROR_BROKEN:
		code_message = "Corrupted data";
		break;
	case ERROR_UNSUPPORTED:
		code_message = "Unsupported feature";
		break;
	case ERROR_UNKNOWN_FORMAT:
		code_message = "Unkhown format";
		break;
	case ERROR_OBSOLETE:
		code_message = "Obsolete feature";
		break;
	case ERROR_ARGUMENT:
		code_message = "Argument error";
		break;
	default:
		code_message = "Unkhown error";
	}

	return fprintf(stderr, "%s : %s%s%s\n", e.function_name, code_message, (explanation) ? ", " : "", explanation);
}
