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

 [file.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "private.h"


int jaConfigurationFile(struct jaConfiguration* config, const char* filename, struct jaStatus* st)
{
	FILE* fp = NULL;
	int ret_value = 0;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaConfigurationFile", JA_STATUS_IO_ERROR, NULL);
		return 1;
	}

	ret_value = jaConfigurationFileEx(config, fp, NULL, st);
	fclose(fp);

	return ret_value;
}


int jaConfigurationFileEx(struct jaConfiguration* config, FILE* fp,
                          void (*warnings_callback)(enum jaStatusCode, int, const char*, const char*),
                          struct jaStatus* st)
{
	(void)config;
	(void)fp;
	(void)warnings_callback;

	jaStatusSet(st, "jaConfigurationFile", JA_STATUS_ERROR, NULL);
	return 1;
}
