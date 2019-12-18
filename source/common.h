/*-----------------------------

 [common.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef COMMON_H
#define COMMON_H

	#ifdef DEBUG
		#include <stdio.h>
		#define DEBUG_PRINT(...) printf(__VA_ARGS__)
	#else
		#define DEBUG_PRINT(...)
	#endif

	#ifdef EXPORT_SYMBOLS
		#ifdef _WIN32
		#define EXPORT         // Whitespace
		#define EXPORT_INLINED // Whitespace
		#else
		#define EXPORT __attribute__((visibility("default")))
		#define EXPORT_INLINED __attribute__((visibility("default"))) inline
		#endif
	#else
		#ifdef _WIN32
		#define EXPORT         // Whitespace
		#define EXPORT_INLINED // Whitespace
		#else
		#define EXPORT // Whitespace
		#define EXPORT_INLINED inline
		#endif
	#endif

#endif
