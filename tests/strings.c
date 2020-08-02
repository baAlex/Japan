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


void StringTokenizerTest1_ASCIISimple(void** cmocka_state)
{
	(void)cmocka_state;

	struct jaStatus st;
	struct jaToken token;
	struct jaTokenizer* t = NULL;

	printf("sizeof(struct jaTokenEnd) = %zu\n", sizeof(struct jaTokenEnd)); // I'm paranoid :/
	printf("sizeof(struct jaToken) = %zu\n", sizeof(struct jaToken));

	// Simple valid tokens, inside an infinite loop
	{
		printf("\n\n");

		t = jaASCIITokenizerCreate((uint8_t*)"cat, dog, bunny, parrot, alpaca, etc.", 38);
		assert_true(t != NULL);

		for (int i = 0;; i++)
		{
			if (jaTokenize(t, &token, &st) != 0)
				break;

			printf("Token '%s', ending with: '%s' (offset: %zu, line: %zu)\n", token.string, token.end_string,
			       token.byte_offset, token.line_number);

			assert_true(i < 6); // Break at six tokens

			switch (i)
			{
			case 0: assert_true(strcmp((char*)token.string, "cat") == 0); break;
			case 1: assert_true(strcmp((char*)token.string, "dog") == 0); break;
			case 2: assert_true(strcmp((char*)token.string, "bunny") == 0); break;
			case 3: assert_true(strcmp((char*)token.string, "parrot") == 0); break;
			case 4: assert_true(strcmp((char*)token.string, "alpaca") == 0); break;
			case 5: assert_true(strcmp((char*)token.string, "etc") == 0); break;
			}
		}

		if (st.code != JA_STATUS_SUCCESS)
			jaStatusPrint(NULL, st);

		assert_true(st.code == JA_STATUS_SUCCESS);
		jaTokenizerDelete(t);
	}

	// With different words, separators, lot of whitespaces and a wrong end
	{
		printf("\n\n");

		t = jaASCIITokenizerCreate((uint8_t*)"cat!? dog# @,bunny-parrot[\t\n  \talpaca]llama;love_etc\t   \t\n", 255);
		assert_true(t != NULL);

		for (int i = 0;; i++)
		{
			if (jaTokenize(t, &token, &st) != 0)
				break;

			printf("Token '%s' (offset: %zu, line: %zu)\n", token.string, token.byte_offset, token.line_number);

			assert_true(i < 7); // Break at seven tokens

			switch (i)
			{
			case 0:
				assert_true(strcmp((char*)token.string, "cat") == 0);
				assert_true(strcmp((char*)token.end_string, "!?") == 0);
				assert_true(token.line_number == 0);
				assert_true(token.byte_offset == 0 && token.byte_offset == token.unit_number);
				break;
			case 1:
				assert_true(strcmp((char*)token.string, "dog") == 0);
				assert_true(strcmp((char*)token.end_string, "#@,") == 0);
				assert_true(token.line_number == 0);
				assert_true(token.byte_offset == 6 && token.byte_offset == token.unit_number);
				break;
			case 2:
				assert_true(strcmp((char*)token.string, "bunny") == 0);
				assert_true(strcmp((char*)token.end_string, "-") == 0);
				assert_true(token.line_number == 0);
				assert_true(token.byte_offset == 13 && token.byte_offset == token.unit_number);
				break;
			case 3:
				assert_true(strcmp((char*)token.string, "parrot") == 0);
				assert_true(strcmp((char*)token.end_string, "[\n") == 0);
				assert_true(token.line_number == 0);
				assert_true(token.byte_offset == 19 && token.byte_offset == token.unit_number);
				break;
			case 4:
				assert_true(strcmp((char*)token.string, "alpaca") == 0);
				assert_true(strcmp((char*)token.end_string, "]") == 0);
				assert_true(token.line_number == 1);
				assert_true(token.byte_offset == 31 && token.byte_offset == token.unit_number);
				break;
			case 5:
				assert_true(strcmp((char*)token.string, "llama") == 0);
				assert_true(strcmp((char*)token.end_string, ";") == 0);
				assert_true(token.line_number == 1);
				assert_true(token.byte_offset == 38 && token.byte_offset == token.unit_number);
				break;
			case 6:
				assert_true(strcmp((char*)token.string, "love_etc") == 0);
				assert_true(strcmp((char*)token.end_string, "\n") == 0);
				assert_true(token.line_number == 1);
				assert_true(token.byte_offset == 44 && token.byte_offset == token.unit_number);
				break; // Underscore don't break!
			}
		}

		if (st.code != JA_STATUS_SUCCESS)
			jaStatusPrint(NULL, st);

		assert_true(st.code == JA_STATUS_SUCCESS);
		jaTokenizerDelete(t);
	}
}


void StringTokenizerTest2_ASCIIRockafeller(void** cmocka_state)
{
	(void)cmocka_state;

	struct jaStatus st;
	struct jaToken token;
	struct jaTokenizer* t = NULL;

	FILE* fp = fopen("./tests/rockafeller.txt", "rb");
	assert_true(fp != NULL);

	uint8_t buffer[ONE_SHOT_BUFFER_LEN];
	size_t buffer_len = fread(buffer, 1, ONE_SHOT_BUFFER_LEN, fp);

	t = jaASCIITokenizerCreate(buffer, buffer_len);
	assert_true(t != NULL);

	int occurrences_right = 0;
	int occurrences_funk = 0;
	int occurrences_soul = 0;
	int occurrences_now = 0;
	int tokens = 0;

	while (1)
	{
		if (jaTokenize(t, &token, &st) != 0)
			break;

		tokens += 1;

		if (strcmp((char*)token.string, "Right") == 0)
			occurrences_right += 1;
		else if (strcmp((char*)token.string, "funk") == 0)
			occurrences_funk += 1;
		else if (strcmp((char*)token.string, "soul") == 0)
			occurrences_soul += 1;
		else if (strcmp((char*)token.string, "now") == 0)
			occurrences_now += 1;
		else if (strcmp((char*)token.string, "Rockafeller") == 0)
		{
			if (token.line_number != 14 && token.line_number != 32 &&
			    token.line_number != 34 & token.line_number != 36 &&
			    token.line_number != 38 & token.line_number != 40 & token.line_number != 42)
			{
				printf("[Error] Rockafeller at line %zu\n", token.line_number);
				assert_true(false);
			}
		}
	}

	printf("'Right' occurrences: %i\n", occurrences_right);
	printf("'funk' occurrences: %i\n", occurrences_funk);
	printf("'soul' occurrences: %i\n", occurrences_soul);
	printf("'now' occurrences: %i\n", occurrences_now);
	printf("%i tokens\n", tokens);

	if (st.code != JA_STATUS_SUCCESS)
		jaStatusPrint(NULL, st);

	assert_true(occurrences_right == 32);
	assert_true(occurrences_funk == 37);
	assert_true(occurrences_soul == 37);
	assert_true(occurrences_now == 121);

	jaTokenizerDelete(t);
	fclose(fp);
}
