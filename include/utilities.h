/*-----------------------------

 [utilities.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.2"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 2
#endif

#ifndef UTILITIES_H
#define UTILITIES_H

	#ifndef M_PI
	#define M_PI 3.14159265358979323846264338327950288f
	#endif

	float DegToRad(float value);
	float RadToDeg(float value);

	int Max_i(int a, int b);
	size_t Max_z(size_t a, size_t b);
	float Max_f(float a, float b);

	int Min_i(int a, int b);
	size_t Min_z(size_t a, size_t b);
	float Min_f(float a, float b);

	int Clamp_i(int v, int min, int max);
	size_t Clamp_z(size_t v, size_t min, size_t max);
	float Clamp_f(float v, float min, float max);


	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define Max(a, b) _Generic((a), \
			int: Max_i, \
			size_t: Max_z, \
			float: Max_f, \
			default: Max_i \
		)(a, b)

	#define Min(a, b) _Generic((a), \
			int: Min_i, \
			size_t: Min_z, \
			float: Min_f, \
			default: Min_i \
		)(a, b)

	#define Clamp(value, min, max) _Generic((value), \
			int: Clamp_i, \
			size_t: Clamp_z, \
			float: Clamp_f, \
			default: Clamp_i \
		)(value, min, max)

	#endif

#endif
