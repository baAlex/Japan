/*-----------------------------

 [strings.c]
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

#include "japan-string.h"


#define LITTLE_BUFFER_LEN 4 // Minimum for a single Unicode unit
#define BIG_BUFFER_LEN (512 * 1024)
#define ONE_SHOT_BUFFER_LEN (512 * 1024)


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
				// Maybe the buffer truncated the unit
				if (unit_bytes != 255 && unit_bytes > buffer_len - i)
				{
					size_t keep = buffer_len - i;

					for (size_t u = 0; u < keep; u++)
						buffer[u] = buffer[i + u];

					buffer_len = fread(buffer + keep, 1, total_buffer_len - keep, fp_in);
					buffer_len += keep;
					goto again;
				}

				// Using '?' since the replacement character is already used
				// by VSCode and Gnome Terminal (where I'm reading the results),
				// this way being capable of identify what my code reject and what not
				putc('?', fp_out);
			}
		}
	}
}


void StringEncodeTest1_Coherency(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_1 = fopen("./tests/out/Kuhn-UTF8-lbuffer.txt", "rb");
	FILE* fp_2 = fopen("./tests/out/Kuhn-UTF8-bbuffer.txt", "rb");
	FILE* fp_3 = fopen("./tests/out/Kuhn-UTF8-one-shot.txt", "rb");
	assert_true(fp_1 != NULL);
	assert_true(fp_2 != NULL);
	assert_true(fp_3 != NULL);

	uint8_t byte_1 = 0;
	uint8_t byte_2 = 0;
	uint8_t byte_3 = 0;
	size_t offset = 0;

	while (1)
	{
		size_t status_1 = fread(&byte_1, 1, 1, fp_1);
		size_t status_2 = fread(&byte_2, 1, 1, fp_2);
		size_t status_3 = fread(&byte_3, 1, 1, fp_3);

		if (status_1 != status_2 || status_1 != status_3 || status_2 != status_3)
		{
			fprintf(stderr, "[Error] Premature ending at offset %zu\n", offset);
			assert_true(0);
		}

		if (byte_1 != byte_2 || byte_1 != byte_3 || byte_2 != byte_3)
		{
			fprintf(stderr, "[Error] Discrepancy at offset %zu (%02X != %02X != %02X)\n", offset, byte_1, byte_2,
			        byte_3);
			assert_true(0);
		}

		if (status_1 == 0)
			break;

		offset += 1;
	}

	fclose(fp_1);
	fclose(fp_2);
	fclose(fp_3);
}


void StringEncodeTest1_KuhnLittleBuffer(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_in = fopen("./tests/Kuhn-UTF8.txt", "rb");
	FILE* fp_out = fopen("./tests/out/Kuhn-UTF8-lbuffer.txt", "wb");
	assert_true(fp_in != NULL);
	assert_true(fp_out != NULL);

	uint8_t buffer[LITTLE_BUFFER_LEN];

	sBufferIteration(buffer, LITTLE_BUFFER_LEN, fp_in, fp_out);

	fclose(fp_out);
	fclose(fp_in);
}


void StringEncodeTest1_KuhnBigBuffer(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_in = fopen("./tests/Kuhn-UTF8.txt", "rb");
	FILE* fp_out = fopen("./tests/out/Kuhn-UTF8-bbuffer.txt", "wb");
	assert_true(fp_in != NULL);
	assert_true(fp_out != NULL);

	uint8_t buffer[BIG_BUFFER_LEN];

	sBufferIteration(buffer, BIG_BUFFER_LEN, fp_in, fp_out);

	fclose(fp_out);
	fclose(fp_in);
}


void StringEncodeTest1_KuhnOneShot(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_in = fopen("./tests/Kuhn-UTF8.txt", "rb");
	FILE* fp_out = fopen("./tests/out/Kuhn-UTF8-one-shot.txt", "wb");
	assert_true(fp_in != NULL);
	assert_true(fp_out != NULL);

	uint8_t buffer[ONE_SHOT_BUFFER_LEN];
	size_t buffer_len = 0;
	size_t i = 0;

	buffer_len = fread(buffer, 1, ONE_SHOT_BUFFER_LEN, fp_in);

	while (1)
	{
		size_t validated_bytes = 0;
		jaUTF8ValidateString(buffer + i, buffer_len - i, &validated_bytes, NULL);

		if (validated_bytes != 0)
		{
			fwrite(buffer + i, 1, validated_bytes, fp_out);
			i += validated_bytes;
		}
		else
		{
			putc('?', fp_out);
			i += 1;
		}

		if (i >= buffer_len)
			break;
	}

	fclose(fp_out);
	fclose(fp_in);
}
