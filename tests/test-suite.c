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

#include <sys/stat.h>
#include <sys/types.h>

#include <cmocka.h>


extern void ListTest1_Iteration(void** cmocka_state);
extern void ListTest2_RemovalAtIteration(void** cmocka_state);
extern void ListTest3_BeforeAfterAddition(void** cmocka_state);

extern void DictionaryTest1_FNV1Hash(void** cmocka_state);
extern void DictionaryTest2_SimpleUsage(void** cmocka_state);

extern void ConfigTest1(void** cmocka_state);
extern void ConfigTest2_TokenizeFile(void** cmocka_state);
extern void ConfigTest3_ParseFile(void** cmocka_state);

extern void ImageTest1_Sgi(void** cmocka_state);

extern void StringEncodeTest1_KuhnBigBuffer(void** cmocka_state);
extern void StringEncodeTest1_KuhnLittleBuffer(void** cmocka_state);
extern void StringEncodeTest1_KuhnOneShot(void** cmocka_state);
extern void StringEncodeTest1_Coherency(void** cmocka_state);

int main()
{

	mkdir("./tests/out/", 0777); // Deprecated on Windows...

	const struct CMUnitTest tests[] = {cmocka_unit_test(ListTest1_Iteration),
	                                   cmocka_unit_test(ListTest2_RemovalAtIteration),
	                                   cmocka_unit_test(ListTest3_BeforeAfterAddition),

	                                   cmocka_unit_test(DictionaryTest1_FNV1Hash),
	                                   cmocka_unit_test(DictionaryTest2_SimpleUsage),

	                                   cmocka_unit_test(ImageTest1_Sgi),

	                                   cmocka_unit_test(ConfigTest1),
	                                   cmocka_unit_test(ConfigTest2_TokenizeFile),
	                                   cmocka_unit_test(ConfigTest3_ParseFile),

	                                   cmocka_unit_test(StringEncodeTest1_KuhnBigBuffer),
	                                   cmocka_unit_test(StringEncodeTest1_KuhnLittleBuffer),
	                                   cmocka_unit_test(StringEncodeTest1_KuhnOneShot),
	                                   cmocka_unit_test(StringEncodeTest1_Coherency)};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
