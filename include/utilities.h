/*-----------------------------

 [utilities.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
#endif

#ifndef UTILITIES_H
#define UTILITIES_H

	#include <stddef.h>

	#ifndef M_PI
	#define M_PI 3.14159265358979323846264338327950288f
	#endif

	#ifndef M_2PI
	#define M_2PI (3.14159265358979323846264338327950288f * 2.0f)
	#endif

	JAPAN_API float DegToRad(float value);
	JAPAN_API float RadToDeg(float value);

	JAPAN_API int MaxInt(int a, int b);
	JAPAN_API size_t MaxSizeT(size_t a, size_t b);
	JAPAN_API float MaxFloat(float a, float b);

	JAPAN_API int MinInt(int a, int b);
	JAPAN_API size_t MinSizeT(size_t a, size_t b);
	JAPAN_API float MinFloat(float a, float b);

	JAPAN_API int ClampInt(int v, int min, int max);
	JAPAN_API size_t ClampSizeT(size_t v, size_t min, size_t max);
	JAPAN_API float ClampFloat(float v, float min, float max);


	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define Max(a, b) _Generic((a), \
			int: MaxInt, \
			size_t: MaxSizeT, \
			float: MaxFloat, \
			default: MaxInt \
		)(a, b)

	#define Min(a, b) _Generic((a), \
			int: MinInt, \
			size_t: MinSizeT, \
			float: MinFloat, \
			default: MinInt \
		)(a, b)

	#define Clamp(value, min, max) _Generic((value), \
			int: ClampInt, \
			size_t: ClampSizeT, \
			float: ClampFloat, \
			default: ClampInt \
		)(value, min, max)

	#endif

#endif
