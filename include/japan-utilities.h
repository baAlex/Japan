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

#include <stddef.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288f
#endif

#ifndef M_2PI
#define M_2PI (3.14159265358979323846264338327950288f * 2.0f)
#endif

JA_EXPORT float jaDegToRad(float value);
JA_EXPORT float jaRadToDeg(float value);

JA_EXPORT int jaMaxInt(int a, int b);
JA_EXPORT size_t jaMaxSizeT(size_t a, size_t b);

JA_EXPORT int jaMinInt(int a, int b);
JA_EXPORT size_t jaMinSizeT(size_t a, size_t b);

JA_EXPORT int jaClampInt(int v, int min, int max);
JA_EXPORT size_t jaClampSizeT(size_t v, size_t min, size_t max);
JA_EXPORT float jaClampFloat(float v, float min, float max);

#if __STDC_VERSION__ >= 201112L

#define jaMax(a, b) _Generic((a), int : jaMaxInt, size_t : jaMaxSizeT, float : fmaxf, default : jaMaxInt)(a, b)
#define jaMin(a, b) _Generic((a), int : jaMinInt, size_t : jaMinSizeT, float : fminf, default : jaMinInt)(a, b)

#define jaClamp(value, min, max)\
	_Generic((value),\
		int : jaClampInt,\
		size_t : jaClampSizeT,\
		float : jaClampFloat,\
		default : jaClampInt\
	)(value, min, max)

#endif

#endif
