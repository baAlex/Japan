/*-----------------------------

 [test-suite.c]
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


extern void ListTest1_Iteration(void** cmocka_state);
extern void ListTest2_RemovalAtIteration(void** cmocka_state);
extern void ListTest3_BeforeAfterAddition(void** cmocka_state);

extern void DictionaryTest1_FNV1Hash(void** cmocka_state);
extern void DictionaryTest2_SimpleUsage(void** cmocka_state);


int main()
{
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(ListTest1_Iteration),
		cmocka_unit_test(ListTest2_RemovalAtIteration),
		cmocka_unit_test(ListTest3_BeforeAfterAddition),

		cmocka_unit_test(DictionaryTest1_FNV1Hash),
		cmocka_unit_test(DictionaryTest2_SimpleUsage)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
