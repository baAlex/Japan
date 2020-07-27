/*-----------------------------

 [encodes.c]
 - Alexander Brandt 2020
-----------------------------*/

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

#include "japan-encode.h"


#define TEST1_BUFFER_LEN 4 // Minimum for a single Unicode unit


void EncodeTest1(void** cmocka_state)
{
	(void)cmocka_state;

	// Mostly works with exception of:
	// - In "Other boundary conditions" it rejects what other applications do not,
	//   while accepting the ones that these reject. :/
	// - UTF-16 surrogates, expected since I'm still didn't wrote any code (TODO)
	// - Internal-use plane, same as above (TODO)

	FILE* fp = fopen("./tests/UTF-8-test.txt", "rb");
	FILE* out = fopen("./utf8-out.txt", "wb");
	assert_true(fp != NULL);
	assert_true(out != NULL);

	uint8_t buffer[TEST1_BUFFER_LEN];
	size_t buffer_len = 0;

	while (1)
	{
		if ((buffer_len = fread(buffer, 1, TEST1_BUFFER_LEN, fp)) == 0)
			break;

		size_t unit_bytes = 0;
	again:
		for (size_t i = 0; i < buffer_len; i++)
		{
			if (jaUTF8ValidateUnit(buffer + i, buffer_len - i, &unit_bytes, NULL) == 0)
			{
				fwrite((buffer + i), 1, unit_bytes + 1, out);
				i += unit_bytes;
			}
			else
			{
				// Maybe the buffer truncated the unit
				if (unit_bytes > buffer_len - i)
				{
					size_t keep = buffer_len - i;

					memcpy(buffer, buffer + i, keep);
					buffer_len = fread(buffer + keep, 1, TEST1_BUFFER_LEN - keep, fp);
					buffer_len += keep;
					goto again;
				}

				// Using hash since the replacement character is already used
				// by VSCode and Gnome Terminal (where I'm reading the results),
				// this way being capable of identify what my code reject and what not
				putc('#', out);
			}
		}
	}

	fclose(out);
	fclose(fp);
}
