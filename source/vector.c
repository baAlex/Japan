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

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


/*-----------------------------

 Vector3
-----------------------------*/
EXPORT inline struct Vector3 Vector3Clean()
{
	return (struct Vector3){0.0, 0.0, 0.0};
}

EXPORT inline struct Vector3 Vector3Set(float x, float y, float z)
{
	return (struct Vector3){x, y, z};
}

EXPORT inline struct Vector3 Vector3Add(struct Vector3 a, struct Vector3 b)
{
	return (struct Vector3){(a.x + b.x), (a.y + b.y), (a.z + b.z)};
}

EXPORT inline struct Vector3 Vector3Subtract(struct Vector3 a, struct Vector3 b)
{
	return (struct Vector3){(a.x - b.x), (a.y - b.y), (a.z - b.z)};
}

EXPORT inline struct Vector3 Vector3Multiply(struct Vector3 a, struct Vector3 b)
{
	return (struct Vector3){(a.x * b.x), (a.y * b.y), (a.z * b.z)};
}

EXPORT inline struct Vector3 Vector3Divide(struct Vector3 a, struct Vector3 b)
{
	return (struct Vector3){(a.x / b.x), (a.y / b.y), (a.z / b.z)};
}

EXPORT inline struct Vector3 Vector3Invert(struct Vector3 v)
{
	return (struct Vector3){(-v.x), (-v.y), (-v.z)};
}

EXPORT inline struct Vector3 Vector3Scale(struct Vector3 v, float scale)
{
	return (struct Vector3){(v.x * scale), (v.y * scale), (v.z * scale)};
}

EXPORT inline struct Vector3 Vector3Normalize(struct Vector3 v)
{
	float length = Vector3Length(v);
	return (struct Vector3){(v.x / length), (v.y / length), (v.z / length)};
}

EXPORT inline float Vector3Length(struct Vector3 v)
{
	return sqrtf(powf(v.x, 2.0) + powf(v.y, 2.0) + powf(v.z, 2.0));
}

EXPORT inline float Vector3Distance(struct Vector3 a, struct Vector3 b)
{
	return sqrtf(powf(b.x - a.x, 2.0) + powf(b.y - a.y, 2.0) + powf(b.z - a.z, 2.0));
}

EXPORT inline bool Vector3Equals(struct Vector3 a, struct Vector3 b)
{
	if (a.x != b.x || a.y != b.y || a.z != b.z)
		return false;

	return true;
}


/*-----------------------------

 Vector4
-----------------------------*/
EXPORT inline struct Vector4 Vector4Clean()
{
	return (struct Vector4){0.0, 0.0, 0.0, 0.0};
}

EXPORT inline struct Vector4 Vector4Set(float x, float y, float z, float u)
{
	return (struct Vector4){x, y, z, u};
}

EXPORT inline struct Vector4 Vector4Add(struct Vector4 a, struct Vector4 b)
{
	return (struct Vector4){(a.x + b.x), (a.y + b.y), (a.z + b.z), (a.u + b.u)};
}

EXPORT inline struct Vector4 Vector4Subtract(struct Vector4 a, struct Vector4 b)
{
	return (struct Vector4){(a.x - b.x), (a.y - b.y), (a.z - b.z), (a.u - b.u)};
}

EXPORT inline struct Vector4 Vector4Multiply(struct Vector4 a, struct Vector4 b)
{
	return (struct Vector4){(a.x * b.x), (a.y * b.y), (a.z * b.z), (a.u * b.u)};
}

EXPORT inline struct Vector4 Vector4Divide(struct Vector4 a, struct Vector4 b)
{
	return (struct Vector4){(a.x / b.x), (a.y / b.y), (a.z / b.z), (a.u / b.u)};
}

EXPORT inline struct Vector4 Vector4Invert(struct Vector4 v)
{
	return (struct Vector4){(-v.x), (-v.y), (-v.z), (-v.u)};
}

EXPORT inline struct Vector4 Vector4Scale(struct Vector4 v, float scale)
{
	return (struct Vector4){(v.x * scale), (v.y * scale), (v.z * scale), (v.u * scale)};
}

EXPORT inline struct Vector4 Vector4Normalize(struct Vector4 v)
{
	float length = Vector4Length(v);
	return (struct Vector4){(v.x / length), (v.y / length), (v.z / length), (v.u / length)};
}

EXPORT inline float Vector4Length(struct Vector4 v)
{
	return sqrtf(powf(v.x, 2.0) + powf(v.y, 2.0) + powf(v.z, 2.0) + powf(v.u, 2.0));
}

EXPORT inline float Vector4Distance(struct Vector4 a, struct Vector4 b)
{
	return sqrtf(powf(b.x - a.x, 2.0) + powf(b.y - a.y, 2.0) + powf(b.z - a.z, 2.0) + powf(b.u - a.u, 2.0));
}

EXPORT inline bool Vector4Equals(struct Vector4 a, struct Vector4 b)
{
	if (a.x != b.x || a.y != b.y || a.z != b.z || a.u != b.u)
		return false;

	return true;
}
