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

 [vector.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <math.h>
#include "vector.h"


inline struct Vector VectorClean()
{
	return (struct Vector){0.0, 0.0, 0.0};
}


inline struct Vector VectorSet(float x, float y, float z)
{
	return (struct Vector){x, y, z};
}


inline struct Vector VectorAdd(struct Vector a, struct Vector b)
{
	return (struct Vector){(a.x + b.x), (a.y + b.y), (a.z + b.z)};
}


inline struct Vector VectorSubtract(struct Vector a, struct Vector b)
{
	return (struct Vector){(a.x - b.x), (a.y - b.y), (a.z - b.z)};
}


inline struct Vector VectorMultiply(struct Vector a, struct Vector b)
{
	return (struct Vector){(a.x * b.x), (a.y * b.y), (a.z * b.z)};
}


inline struct Vector VectorDivide(struct Vector a, struct Vector b)
{
	return (struct Vector){(a.x / b.x), (a.y / b.y), (a.z / b.z)};
}


inline struct Vector VectorInvert(struct Vector v)
{
	return (struct Vector){(-v.x), (-v.y), (-v.z)};
}


inline struct Vector VectorScale(struct Vector v, float scale)
{
	return (struct Vector){(v.x * scale), (v.y * scale), (v.z * scale)};
}


inline struct Vector VectorNormalize(struct Vector v)
{
	float length = VectorLength(v);
	return (struct Vector){(v.x / length), (v.y / length), (v.z / length)};
}


inline float VectorLength(struct Vector v)
{
	return sqrtf(powf(v.x, 2.0) + powf(v.y, 2.0) + powf(v.z, 2.0));
}


inline float VectorDistance(struct Vector a, struct Vector b)
{
	return sqrtf(powf(b.x - a.x, 2.0) + powf(b.y - a.y, 2.0) + powf(b.z - a.z, 2.0));
}


inline bool VectorEquals(struct Vector a, struct Vector b)
{
	if (a.x != b.x || a.y != b.y || a.z != b.z)
		return false;

	return true;
}
