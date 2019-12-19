/*-----------------------------

 [test-suite.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

extern void ListTest1_007Best(void** cmocka_state);
extern void ListTest2_007Worst(void** cmocka_state);
extern void ListTest3_007Quote(void** cmocka_state);


int main()
{
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(ListTest1_007Best),
		cmocka_unit_test(ListTest2_007Worst),
		cmocka_unit_test(ListTest3_007Quote)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
