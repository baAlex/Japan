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


#define LITTLE_BUFFER_LEN 4 // Minimum for a single Unicode unit
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


void EncodeTest1_BigLittleCoherency(void** cmocka_state)
{
	(void)cmocka_state;

	FILE* fp_l = fopen("./tests/out/Kuhn-UTF8-lbuffer.txt", "rb");
	FILE* fp_b = fopen("./tests/out/Kuhn-UTF8-bbuffer.txt", "rb");
	assert_true(fp_l != NULL);
	assert_true(fp_b != NULL);

	uint8_t byte_l = 0;
	uint8_t byte_b = 0;
	size_t offset = 0;

	while (1)
	{
		size_t status_l = fread(&byte_l, 1, 1, fp_l);
		size_t status_b = fread(&byte_b, 1, 1, fp_b);

		if (status_l != status_b)
		{
			fprintf(stderr, "[Error] Premature ending at offset %zu\n", offset);
			assert_true(0);
		}

		if (byte_l != byte_b)
		{
			fprintf(stderr, "[Error] Discrepancy at offset %zu (%02X != %02X)\n", offset, byte_l, byte_b);
			assert_true(0);
		}

		if (status_l == 0)
			break;

		offset += 1;
	}

	fclose(fp_l);
	fclose(fp_b);
}


void EncodeTest1_KuhnLittleBuffer(void** cmocka_state)
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


void EncodeTest1_KuhnBigBuffer(void** cmocka_state)
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
