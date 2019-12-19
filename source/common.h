/*-----------------------------

 [common.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JA_COMMON_H
#define JA_COMMON_H

	#ifdef JA_DEBUG
		#include <stdio.h>
		#define JA_DEBUG_PRINT(...) printf(__VA_ARGS__)
	#else
		#define JA_DEBUG_PRINT(...)
	#endif

#endif
