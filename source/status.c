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
 - Alexander Brandt 2019-2020
-----------------------------*/

#include <stdio.h>
#include <string.h>

#include "japan-status.h"

void jaStatusSet(struct jaStatus* st, const char* function_name, enum jaStatusCode c, const char* explanation_fmt, ...)
{
	va_list args;

	if (st == NULL)
		return;

	st->code = c;

	if (function_name != NULL)
		strncpy(st->function_name, function_name, JA_STATUS_FNAME_LEN);

	if (explanation_fmt != NULL)
	{
		va_start(args, explanation_fmt);
		vsnprintf(st->explanation, JA_STATUS_EXPL_LEN, explanation_fmt, args);
		va_end(args);
	}
}


inline void jaStatusCopy(const struct jaStatus* org, struct jaStatus* dest)
{
	if (org != NULL && dest != NULL)
		memcpy(dest, org, sizeof(struct jaStatus));
}


int jaStatusPrint(const char* app_name, struct jaStatus st)
{
	char* explanation = (st.explanation[0] != '\0') ? st.explanation : "";
	char* code_message = NULL;

	switch (st.code)
	{
	case JA_STATUS_SUCCESS: code_message = "Success"; break;
	case JA_STATUS_ERROR: code_message = "Error"; break;
	case JA_STATUS_FS_ERROR: code_message = "Filesystem error"; break;
	case JA_STATUS_IO_ERROR: code_message = "File Input/Output error"; break;
	case JA_STATUS_UNEXPECTED_EOF: code_message = "End of file prematurely reached"; break;
	case JA_STATUS_UNEXPECTED_DATA: code_message = "Unexpected data"; break;
	case JA_STATUS_UNKNOWN_FILE_FORMAT: code_message = "Unknown file format"; break;
	case JA_STATUS_UNSUPPORTED_FEATURE: code_message = "Unsupported feature"; break;
	case JA_STATUS_OBSOLETE_FEATURE: code_message = "Obsolete feature"; break;
	case JA_STATUS_UNKNOWN_DATA_FORMAT: code_message = "Unknown data format"; break;
	case JA_STATUS_INVALID_ARGUMENT: code_message = "Invalid argument"; break;
	case JA_STATUS_MEMORY_ERROR: code_message = "Memory error"; break;
	case JA_STATUS_EXPECTED_KEY_TOKEN: code_message = "Unknown token, expected a key name"; break;
	case JA_STATUS_EXPECTED_EQUAL_TOKEN: code_message = "Unknown token, expected an equal sign"; break;
	case JA_STATUS_STATEMENT_OPEN: code_message = "Statement remains open"; break;
	case JA_STATUS_NO_ASSIGNMENT: code_message = "Statement didn't have an assignment"; break;
	case JA_STATUS_INTEGER_CAST_ERROR: code_message = "Can't cast into an integer"; break;
	case JA_STATUS_DECIMAL_CAST_ERROR: code_message = "Can't cast into a decimal"; break;
	case JA_STATUS_ASCII_ERROR: code_message = "Invalid ASCII"; break;
	case JA_STATUS_UTF8_ERROR: code_message = "Invalid UTF8"; break;

	default: code_message = "Unknown status";
	}

	return fprintf(stderr, "[%s] %s : %s%s%s.\n", app_name, st.function_name, code_message,
	               (st.explanation[0] != '\0') ? ", " : "", explanation);
}
