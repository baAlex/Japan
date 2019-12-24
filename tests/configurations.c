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

	jaConfigRegister(cfg, "r_width", 640, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "s_volume", 0.8f, 0.0f, 1.0f, NULL);
	jaConfigRegister(cfg, "r_height", 320, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "name", "Ranger", NULL, NULL, NULL);
	jaConfigRegister(cfg, "r_fullscreen", 0, 0, 1, NULL);

	// Read arguments, that includes whitespaces, out
	// of range and incorrecty typed values
	const char* v[] = {"", "-r_height", " X3  ", "-r_width", "   200.2  ", "-s_volume",
	                   "  +0.4 6", "-r_fullscreen", "2  ", "-name", "OwO", "UwU"};

	jaConfigReadArguments(cfg, 12, v, CONFIG_DEFAULT);

	// And check
	union {
		int i;
		float f;
		const char* s;
	} value;

	jaConfigRetrieve(cfg, "s_volume", &value.f, NULL);
	assert_true((value.f == 0.8f)); // Value of "  +0.4 6" can't be cast into a float

	jaConfigRetrieve(cfg, "name", &value.s, NULL);
	assert_string_equal(value.s, "OwO");

	jaConfigRetrieve(cfg, "r_width", &value.i, NULL);
	assert_int_equal(value.i, 200); // Round of value "   200.2  "

	jaConfigRetrieve(cfg, "r_height", &value.i, NULL);
	assert_int_equal(value.i, 320); // Value of " UwU  " can't be cast into a integer

	jaConfigRetrieve(cfg, "r_fullscreen", &value.i, NULL);
	assert_int_equal(value.i, 1); // Value of "2" gets clamp

	// Bye!
	jaConfigDelete(cfg);
}


/*-----------------------------

 ConfigTest2()
-----------------------------*/
void ConfigTest2(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaStatus st = {0};

	// Create a configuration
	struct jaConfig* cfg = jaConfigCreate();

	jaConfigRegister(cfg, "r_width", 640, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "s_volume", 0.8f, 0.0f, 1.0f, NULL);
	jaConfigRegister(cfg, "r_height", 320, 0, INT_MAX, NULL);
	jaConfigRegister(cfg, "name", "Ranger", NULL, NULL, NULL);
	jaConfigRegister(cfg, "r_fullscreen", 0, 0, 1, NULL);

	// Read a file
	if(jaConfigReadFile(cfg, "./tests/test.cfg", &st) != 0)
		jaStatusPrint("", st);

	// Bye!
	jaConfigDelete(cfg);
}
