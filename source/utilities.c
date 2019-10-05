/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [utilities.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utilities.h"


EXPORT inline float DegToRad(float value)
{
	return value * M_PI / 180.0f;
}

EXPORT inline float RadToDeg(float value)
{
	return value * 180.0f / M_PI;
}


EXPORT inline int Max_i(int a, int b)
{
	return (a > b) ? a : b;
}

EXPORT inline size_t Max_z(size_t a, size_t b)
{
	return (a > b) ? a : b;
}

EXPORT inline float Max_f(float a, float b)
{
	return (a > b) ? a : b;
}


EXPORT inline int Min_i(int a, int b)
{
	return (a < b) ? a : b;
}

EXPORT inline size_t Min_z(size_t a, size_t b)
{
	return (a < b) ? a : b;
}

EXPORT inline float Min_f(float a, float b)
{
	return (a < b) ? a : b;
}


EXPORT inline int Clamp_i(int v, int min, int max)
{
	if (v > max)
		return max;

	if (v < min)
		return min;

	return v;
}

EXPORT inline size_t Clamp_z(size_t v, size_t min, size_t max)
{
	if (v > max)
		return max;

	if (v < min)
		return min;

	return v;
}

EXPORT inline float Clamp_f(float v, float min, float max)
{
	if (v > max)
		return max;

	if (v < min)
		return min;

	return v;
}
