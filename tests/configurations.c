/*-----------------------------

 [configurations.c]
 - Alexander Brandt 2019
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

#include "japan-config.h"

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
	struct jaConfig* cfg = jaConfigCreate();

	jaConfigRegister(cfg, "render.width", 640, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "sound.volume", 0.8f, 0.0f, 1.0f, NULL);
	jaConfigRegister(cfg, "render.height", 320, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "name", "Ranger", NULL, NULL, NULL);
	jaConfigRegister(cfg, "render.fullscreen", 0, 0, 1, NULL);

	// Read arguments, that includes whitespaces, out
	// of range and incorrecty typed values
	const char* v[] = {"", "-render.height", " X3  ", "-render.width", "   200.2  ", "-sound.volume",
	                   "  +0.4 6", "-render.fullscreen", "2  ", "-name", "OwO", "UwU"};

	jaConfigReadArguments(cfg, 12, v, NULL);

	// And check
	union {
		int i;
		float f;
		const char* s;
	} value;

	jaConfigRetrieve(cfg, "sound.volume", &value.f, NULL);
	assert_true((value.f == 0.8f)); // Value of "  +0.4 6" can't be cast into a float

	jaConfigRetrieve(cfg, "name", &value.s, NULL);
	assert_string_equal(value.s, "OwO");

	jaConfigRetrieve(cfg, "render.width", &value.i, NULL);
	assert_int_equal(value.i, 200); // Round of value "   200.2  "

	jaConfigRetrieve(cfg, "render.height", &value.i, NULL);
	assert_int_equal(value.i, 320); // Value of " UwU  " can't be cast into a integer

	jaConfigRetrieve(cfg, "render.fullscreen", &value.i, NULL);
	assert_int_equal(value.i, 1); // Value of "2" gets clamp

	// Bye!
	jaConfigDelete(cfg);
}


/*-----------------------------

 ConfigTest2_TokenizeFile()
-----------------------------*/
void ConfigTest2_TokenizeFile(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaStatus st = {0};

	struct jaConfig* cfg = jaConfigCreate();
	FILE* fp = fopen("./tests/tokenize-test.jcfg", "rb");

	if (jaConfigReadFileEx(cfg, fp, FILE_TOKENIZE_ONLY, &st) != 0)
		jaStatusPrint("", st);

	// Bye!
	fclose(fp);
	jaConfigDelete(cfg);
}


/*-----------------------------

 ConfigTest3_ParseFile()
-----------------------------*/
void ConfigTest3_ParseFile(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaStatus st = {0};

	struct jaConfig* cfg = jaConfigCreate();
	FILE* fp = fopen("./tests/parse-test.jcfg", "rb");

	jaConfigRegister(cfg, "osc.shape", "square", NULL, NULL, NULL);
	jaConfigRegister(cfg, "osc.frequency", 220, INT_MIN, INT_MAX, NULL);
	jaConfigRegister(cfg, "osc.sub_volume", 0.5f, 0.0f, 1.0f, NULL);

	jaConfigRegister(cfg, "env.attack", 50.0f, 0.0f, 1000.0f, NULL);
	jaConfigRegister(cfg, "env.decay", 0.0f, 0.0f, 1000.0f, NULL);
	jaConfigRegister(cfg, "env.sustain", 1.0f, 0.0f, 1.0f, NULL);
	jaConfigRegister(cfg, "env.hold", 0.0f, 0.0f, 1000.0f, NULL);
	jaConfigRegister(cfg, "env.release", 200.0f, 0.0f, 1000.0f, NULL);

	if (jaConfigReadFileEx(cfg, fp, FILE_DEFAULT, &st) != 0)
		jaStatusPrint("", st);

	// Bye!
	fclose(fp);
	jaConfigDelete(cfg);
}
