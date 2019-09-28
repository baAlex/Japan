/*-----------------------------

 [common.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef COMMON_H
#define COMMON_H

	#ifdef DEBUG
		#include <stdio.h>
		#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
	#else
		#define DEBUG_PRINT(fmt, ...)
	#endif

	#ifdef EXPORT_SYMBOLS
		#define EXPORT __attribute__((visibility("default")))
	#else
		#define EXPORT // Whitespace
	#endif

#endif
