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


#define LITTLE_BUFFER_LEN 4 // For a single Unicode unit
#define BIG_BUFFER_LEN (512 * 1024)


static void sBufferIteration(uint8_t* buffer, size_t total_buffer_len, FILE* fp_in, FILE* fp_out)
{
	size_t buffer_len = 0;

	while (1)
	{
		if ((buffer_len = fread(buffer, 1, total_buffer_len, fp_in)) == 0)
			break;

		size_t unit_bytes = 0;

	again:
		for (size_t i = 0; i < buffer_len; i++)
		{
			if (jaUTF8ValidateUnit(buffer + i, buffer_len - i, &unit_bytes, NULL) == 0)
			{
				fwrite((buffer + i), 1, unit_bytes, fp_out);
				i += unit_bytes - 1;
			}
			else
			{
				// Bug- BBUUUUUUGGGGGG!!!!!!

				// Maybe the buffer truncated the unit
				/*if (unit_bytes > buffer_len - i)
				{
				    size_t keep = buffer_len - i;

				    memcpy(buffer, buffer + i, keep);
				    buffer_len = fread(buffer + keep, 1, total_buffer_len - keep, fp_in);
				    buffer_len += keep;
				    goto again;
				}*/

				// Using '?' since the replacement character is already used
				// by VSCode and Gnome Terminal (where I'm reading the results),
				// this way being capable of identify what my code reject and what not
				putc('?', fp_out);
			}
		}
	}
}


void EncodeTest1_KuhnLittleBuffer(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_in = fopen("./tests/UTF-8-test.txt", "rb");
	FILE* fp_out = fopen("./Kuhn-UTF8-little.txt", "wb");
	assert_true(fp_in != NULL);
	assert_true(fp_out != NULL);

	uint8_t buffer[LITTLE_BUFFER_LEN];

	sBufferIteration(buffer, LITTLE_BUFFER_LEN, fp_in, fp_out);

	fclose(fp_out);
	fclose(fp_in);
}


void EncodeTest1_KuhnBigBuffer(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_in = fopen("./tests/UTF-8-test.txt", "rb");
	FILE* fp_out = fopen("./Kuhn-UTF8-big.txt", "wb");
	assert_true(fp_in != NULL);
	assert_true(fp_out != NULL);

	uint8_t buffer[BIG_BUFFER_LEN];

	sBufferIteration(buffer, BIG_BUFFER_LEN, fp_in, fp_out);

	fclose(fp_out);
	fclose(fp_in);
}
