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
 - Alexander Brandt 2019-2020
-----------------------------*/

#include <math.h>

#include "japan-utilities.h"
#include "japan-vector.h"


inline struct jaVectorF2 jaVectorAddF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (struct jaVectorF2){(a.x + b.x), (a.y + b.y)};
}


inline struct jaVectorF2 jaVectorSubtractF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (struct jaVectorF2){(a.x - b.x), (a.y - b.y)};
}


inline struct jaVectorF2 jaVectorMultiplyF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (struct jaVectorF2){(a.x * b.x), (a.y * b.y)};
}


inline struct jaVectorF2 jaVectorDivideF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (struct jaVectorF2){(a.x / b.x), (a.y / b.y)};
}


inline struct jaVectorF2 jaVectorInvertF2(struct jaVectorF2 v)
{
	return (struct jaVectorF2){(-v.x), (-v.y)};
}


inline struct jaVectorF2 jaVectorScaleF2(struct jaVectorF2 v, float scale)
{
	return (struct jaVectorF2){(v.x * scale), (v.y * scale)};
}


inline struct jaVectorF2 jaVectorNormalizeF2(struct jaVectorF2 v)
{
	float length = jaVectorLengthF2(v);
	return (struct jaVectorF2){(v.x / length), (v.y / length)};
}


inline float jaVectorCrossF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (a.x * b.y) - (a.y * b.x);
}


inline float jaVectorLengthF2(struct jaVectorF2 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f));
}


inline float jaVectorDistanceF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f));
}


inline float jaVectorDotF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	return (a.x * b.x) + (a.y * b.y);
}


inline bool jaVectorEqualsF2(struct jaVectorF2 a, struct jaVectorF2 b)
{
	if (fabsf(a.x - b.x) > JA_EQUALS_EPSILON || fabsf(a.y - b.y) > JA_EQUALS_EPSILON)
		return false;

	return true;
}


inline struct jaVectorF3 jaVectorAddF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (struct jaVectorF3){(a.x + b.x), (a.y + b.y), (a.z + b.z)};
}


inline struct jaVectorF3 jaVectorSubtractF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (struct jaVectorF3){(a.x - b.x), (a.y - b.y), (a.z - b.z)};
}


inline struct jaVectorF3 jaVectorMultiplyF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (struct jaVectorF3){(a.x * b.x), (a.y * b.y), (a.z * b.z)};
}


inline struct jaVectorF3 jaVectorDivideF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (struct jaVectorF3){(a.x / b.x), (a.y / b.y), (a.z / b.z)};
}


inline struct jaVectorF3 jaVectorInvertF3(struct jaVectorF3 v)
{
	return (struct jaVectorF3){(-v.x), (-v.y), (-v.z)};
}


inline struct jaVectorF3 jaVectorScaleF3(struct jaVectorF3 v, float scale)
{
	return (struct jaVectorF3){(v.x * scale), (v.y * scale), (v.z * scale)};
}


inline struct jaVectorF3 jaVectorNormalizeF3(struct jaVectorF3 v)
{
	float length = jaVectorLengthF3(v);
	return (struct jaVectorF3){(v.x / length), (v.y / length), (v.z / length)};
}


inline struct jaVectorF3 jaVectorCrossF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (struct jaVectorF3){(a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x)};
}


inline float jaVectorLengthF3(struct jaVectorF3 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f));
}


inline float jaVectorDistanceF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f) + powf(b.z - a.z, 2.0f));
}


inline float jaVectorDotF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


inline bool jaVectorEqualsF3(struct jaVectorF3 a, struct jaVectorF3 b)
{
	if (fabsf(a.x - b.x) > JA_EQUALS_EPSILON || fabsf(a.y - b.y) > JA_EQUALS_EPSILON ||
	    fabsf(a.z - b.z) > JA_EQUALS_EPSILON)
		return false;

	return true;
}


inline struct jaVectorF4 jaVectorAddF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return (struct jaVectorF4){(a.x + b.x), (a.y + b.y), (a.z + b.z), (a.w + b.w)};
}


inline struct jaVectorF4 jaVectorSubtractF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return (struct jaVectorF4){(a.x - b.x), (a.y - b.y), (a.z - b.z), (a.w - b.w)};
}


inline struct jaVectorF4 jaVectorMultiplyF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return (struct jaVectorF4){(a.x * b.x), (a.y * b.y), (a.z * b.z), (a.w * b.w)};
}


inline struct jaVectorF4 jaVectorDivideF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return (struct jaVectorF4){(a.x / b.x), (a.y / b.y), (a.z / b.z), (a.w / b.w)};
}


inline struct jaVectorF4 jaVectorInvertF4(struct jaVectorF4 v)
{
	return (struct jaVectorF4){(-v.x), (-v.y), (-v.z), (-v.w)};
}


inline struct jaVectorF4 jaVectorScaleF4(struct jaVectorF4 v, float scale)
{
	return (struct jaVectorF4){(v.x * scale), (v.y * scale), (v.z * scale), (v.w * scale)};
}


inline struct jaVectorF4 jaVectorNormalizeF4(struct jaVectorF4 v)
{
	float length = jaVectorLengthF4(v);
	return (struct jaVectorF4){(v.x / length), (v.y / length), (v.z / length), (v.w / length)};
}


inline float jaVectorLengthF4(struct jaVectorF4 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f) + powf(v.w, 2.0f));
}


inline float jaVectorDistanceF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f) + powf(b.z - a.z, 2.0f) + powf(b.w - a.w, 2.0f));
}


inline float jaVectorDotF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}


inline bool jaVectorEqualsF4(struct jaVectorF4 a, struct jaVectorF4 b)
{
	if (fabsf(a.x - b.x) > JA_EQUALS_EPSILON || fabsf(a.y - b.y) > JA_EQUALS_EPSILON ||
	    fabsf(a.z - b.z) > JA_EQUALS_EPSILON || fabsf(a.w - b.w) > JA_EQUALS_EPSILON)
		return false;

	return true;
}
