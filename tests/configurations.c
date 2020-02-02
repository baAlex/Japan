/*-----------------------------

 [configurations.c]
 - Alexander Brandt 2019-2020
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

#include "japan-configuration.h"

// TODO, check errors
// TODO, check errors status
// TODO, this test isn't really trying to break anything!


/*-----------------------------

 ConfigTest1()
-----------------------------*/
void ConfigTest1(void** cmocka_state)
{
	(void)cmocka_state;

	// Create a configuration
	struct jaConfiguration* cfg = jaConfigurationCreate();

	jaCvarCreate(cfg, "render.width", 640, 0, INT_MAX, NULL);
	jaCvarCreate(cfg, "sound.volume", 0.8f, 0.0f, 1.0f, NULL);
	jaCvarCreate(cfg, "render.height", 320, 0, INT_MAX, NULL);
	jaCvarCreate(cfg, "name", "Ranger", NULL, NULL, NULL);
	jaCvarCreate(cfg, "render.fullscreen", 0, 0, 1, NULL);

	// Read arguments, that includes whitespaces, out
	// of range and incorrecty typed values
	const char* v[] = {"", "-render.height", " X3  ", "-render.width", "   200.2  ", "-sound.volume",
	                   "  +0.4 6", "-render.fullscreen", "2  ", "-name", "OwO", "UwU"};

	jaConfigurationArguments(cfg, 12, v);

	// And check
	union {
		int i;
		float f;
		const char* s;
	} value;

	jaCvarValue(jaCvarGet(cfg, "sound.volume"), &value.f, NULL);
	assert_true((value.f == 0.8f)); // Value of "  +0.4 6" can't be cast into a float

	jaCvarValue(jaCvarGet(cfg, "name"), &value.s, NULL);
	assert_string_equal(value.s, "OwO");

	jaCvarValue(jaCvarGet(cfg, "render.width"), &value.i, NULL);
	assert_int_equal(value.i, 200); // Round of value "   200.2  "

	jaCvarValue(jaCvarGet(cfg, "render.height"), &value.i, NULL);
	assert_int_equal(value.i, 320); // Value of " UwU  " can't be cast into a integer

	jaCvarValue(jaCvarGet(cfg, "render.fullscreen"), &value.i, NULL);
	assert_int_equal(value.i, 1); // Value of "2" gets clamp

	// Bye!
	jaConfigurationDelete(cfg);
}


/*-----------------------------

 ConfigTest2_TokenizeFile()
-----------------------------*/
void ConfigTest2_TokenizeFile(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaStatus st = {0};

	struct jaConfiguration* cfg = jaConfigurationCreate();
	FILE* fp = fopen("./tests/tokenize-test.jcfg", "rb");

	if (jaConfigurationFileEx(cfg, fp, FILE_TOKENIZE_ONLY, NULL, NULL, &st) != 0)
		jaStatusPrint("", st);

	// Bye!
	fclose(fp);
	jaConfigurationDelete(cfg);
}


/*-----------------------------

 ConfigTest3_ParseFile()
-----------------------------*/
void sTokenizerCallback(int line_no, struct jaTokenDelimiter delimiter, const char* token)
{
	//printf("(b:%s%s%s) %04i: \"%s\"\n", (delimiter.ws) ? "ws" : "--", (delimiter.nl) ? "nl" : "--",
	//       (delimiter.sc) ? "sc" : "--", line_no, token);
}

void sWarningsCallback(enum jaStatusCode code, int line_no, const char* token, const char* key)
{
	switch (code)
	{
	case STATUS_EXPECTED_KEY_TOKEN:
		printf("[Warning] Expected an configuration key in line %i, found the unknown token '%s' instead\n", line_no,
		       token);
		break;
	case STATUS_EXPECTED_EQUAL_TOKEN:
		printf("[Warning] Expected an equal sign for key '%s' in line %i, found the token '%s' instead\n", key, line_no,
		       token);
		break;
	case STATUS_STATEMENT_OPEN:
		printf("[Warning] Statement in line %i isn't properly closed, '%s' assignament ignored\n", line_no, key);
		break;
	case STATUS_NO_ASSIGNMENT:
		printf("[Warning] Configuration '%s' in line %i did't have a value assigned\n", key, line_no);
		break;
	default: break;
	}
}

void ConfigTest3_ParseFile(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaStatus st = {0};

	struct jaConfiguration* cfg = jaConfigurationCreate();
	FILE* fp = fopen("./tests/parse-test.jcfg", "rb");

	jaCvarCreate(cfg, "osc.shape", "square", NULL, NULL, NULL);
	jaCvarCreate(cfg, "osc.frequency", 220, INT_MIN, INT_MAX, NULL);
	jaCvarCreate(cfg, "osc.sub_volume", 0.5f, 0.0f, 1.0f, NULL);

	jaCvarCreate(cfg, "env.attack", 50.0f, 0.0f, 1000.0f, NULL);
	jaCvarCreate(cfg, "env.decay", 0.0f, 0.0f, 1000.0f, NULL);
	jaCvarCreate(cfg, "env.sustain", 1.0f, 0.0f, 1.0f, NULL);
	jaCvarCreate(cfg, "env.hold", 0.0f, 0.0f, 1000.0f, NULL);
	jaCvarCreate(cfg, "env.release", 200.0f, 0.0f, 1000.0f, NULL);

	if (jaConfigurationFileEx(cfg, fp, FILE_DEFAULT, sTokenizerCallback, sWarningsCallback, &st) != 0)
		jaStatusPrint("", st);

	// Bye!
	fclose(fp);
	jaConfigurationDelete(cfg);
}
