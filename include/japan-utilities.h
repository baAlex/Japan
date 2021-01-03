/*-----------------------------

 [japan-utilities.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_UTILITIES_H
#define JAPAN_UTILITIES_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <math.h>
#include <stddef.h>


// https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/math.h.html
// https://github.com/llvm/llvm-project/blob/master/libclc/generic/include/clc/float/definitions.h

#ifndef M_PI_F
#define M_PI_F 0x1.921fb6p+1f
#endif
#ifndef M_PI_2_F
#define M_PI_2_F 0x1.921fb6p+0f
#endif
#ifndef M_PI_4_F
#define M_PI_4_F 0x1.921fb6p-1f
#endif
#ifndef M_1_PI_F
#define M_1_PI_F 0x1.45f306p-2f
#endif
#ifndef M_2_PI_F
#define M_2_PI_F 0x1.45f306p-1f
#endif

#define JA_EQUALS_EPSILON 0.000015259f // 1 / (2^16 − 1)
// #define JA_EQUALS_EPSILON 0.003921569f // 1 / (2^8 - 1)

JA_EXPORT float jaDegToRad(float value);
JA_EXPORT float jaRadToDeg(float value);

JA_EXPORT int jaMaxI(int a, int b);
JA_EXPORT size_t jaMaxZ(size_t a, size_t b);

JA_EXPORT int jaMinI(int a, int b);
JA_EXPORT size_t jaMinZ(size_t a, size_t b);

JA_EXPORT int jaClampI(int v, int min, int max);
JA_EXPORT size_t jaClampZ(size_t v, size_t min, size_t max);

JA_EXPORT float jaClampF(float v, float min, float max);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaMax(a, b) _Generic((a), int : jaMaxI, size_t : jaMaxZ, float : fmaxf, default : jaMaxI)(a, b)
#define jaMin(a, b) _Generic((a), int : jaMinI, size_t : jaMinZ, float : fminf, default : jaMinI)(a, b)

#define jaClamp(value, min, max)\
	_Generic((value),\
		int : jaClampI,\
		size_t : jaClampZ,\
		float : jaClampF,\
		default : jaClampI\
	)(value, min, max)

#endif

#endif
