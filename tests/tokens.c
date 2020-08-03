/*-----------------------------

 [tokens.c]
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

#include "japan-token.h"

#define ONE_SHOT_BUFFER_LEN (512 * 1024)

void TokenizerTest1_ASCIISimple(void** cmocka_state)
{
	(void)cmocka_state;

	struct jaStatus st;
	struct jaToken token;
	struct jaTokenizer* t = NULL;

	printf("sizeof(struct jaTokenEnd) = %zu\n", sizeof(struct jaTokenEnd)); // I'm paranoid :/
	printf("sizeof(struct jaToken) = %zu\n", sizeof(struct jaToken));

	// Simple valid tokens, inside an infinite loop
	{
		printf("\n");

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
		printf("\n");

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

	// Invalid characters
	{
		printf("\n");

		uint8_t string[] = {128, 129, 255, 0x00};
		t = jaASCIITokenizerCreate(string, 255);
		assert_true(t != NULL);

		for (int i = 0;; i++)
		{
			if (jaTokenize(t, &token, &st) != 0)
				break;

			assert_true(i < 1);
		}

		if (st.code != JA_STATUS_SUCCESS)
			jaStatusPrint(NULL, st);

		assert_true(st.code != JA_STATUS_SUCCESS);
		jaTokenizerDelete(t);
	}
}


void TokenizerTest2_ASCIIRockafeller(void** cmocka_state)
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
