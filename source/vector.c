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

#include "japan-vector.h"
#include <math.h>


/*-----------------------------

 Vector2
-----------------------------*/
inline struct jaVector2 jaVector2Clean()
{
	return (struct jaVector2){0.0f, 0.0f};
}


inline struct jaVector2 jaVector2Set(float x, float y)
{
	return (struct jaVector2){x, y};
}


inline struct jaVector2 jaVector2Add(struct jaVector2 a, struct jaVector2 b)
{
	return (struct jaVector2){(a.x + b.x), (a.y + b.y)};
}


inline struct jaVector2 jaVector2Subtract(struct jaVector2 a, struct jaVector2 b)
{
	return (struct jaVector2){(a.x - b.x), (a.y - b.y)};
}


inline struct jaVector2 jaVector2Multiply(struct jaVector2 a, struct jaVector2 b)
{
	return (struct jaVector2){(a.x * b.x), (a.y * b.y)};
}


inline struct jaVector2 jaVector2Divide(struct jaVector2 a, struct jaVector2 b)
{
	return (struct jaVector2){(a.x / b.x), (a.y / b.y)};
}


inline struct jaVector2 jaVector2Invert(struct jaVector2 v)
{
	return (struct jaVector2){(-v.x), (-v.y)};
}


inline struct jaVector2 jaVector2Scale(struct jaVector2 v, float scale)
{
	return (struct jaVector2){(v.x * scale), (v.y * scale)};
}


inline struct jaVector2 jaVector2Normalize(struct jaVector2 v)
{
	float length = jaVector2Length(v);
	return (struct jaVector2){(v.x / length), (v.y / length)};
}


inline float jaVector2Cross(struct jaVector2 a, struct jaVector2 b)
{
	return (a.x * b.y) - (a.y * b.x);
}


inline float jaVector2Length(struct jaVector2 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f));
}


inline float jaVector2Distance(struct jaVector2 a, struct jaVector2 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f));
}


inline float jaVector2Dot(struct jaVector2 a, struct jaVector2 b)
{
	return (a.x * b.x) + (a.y * b.y);
}


inline bool jaVector2Equals(struct jaVector2 a, struct jaVector2 b)
{
	if (a.x != b.x || a.y != b.y)
		return false;

	return true;
}


/*-----------------------------

 Vector3
-----------------------------*/
inline struct jaVector3 jaVector3Clean()
{
	return (struct jaVector3){0.0f, 0.0f, 0.0f};
}


inline struct jaVector3 jaVector3Set(float x, float y, float z)
{
	return (struct jaVector3){x, y, z};
}


inline struct jaVector3 jaVector3Add(struct jaVector3 a, struct jaVector3 b)
{
	return (struct jaVector3){(a.x + b.x), (a.y + b.y), (a.z + b.z)};
}


inline struct jaVector3 jaVector3Subtract(struct jaVector3 a, struct jaVector3 b)
{
	return (struct jaVector3){(a.x - b.x), (a.y - b.y), (a.z - b.z)};
}


inline struct jaVector3 jaVector3Multiply(struct jaVector3 a, struct jaVector3 b)
{
	return (struct jaVector3){(a.x * b.x), (a.y * b.y), (a.z * b.z)};
}


inline struct jaVector3 jaVector3Divide(struct jaVector3 a, struct jaVector3 b)
{
	return (struct jaVector3){(a.x / b.x), (a.y / b.y), (a.z / b.z)};
}


inline struct jaVector3 jaVector3Invert(struct jaVector3 v)
{
	return (struct jaVector3){(-v.x), (-v.y), (-v.z)};
}


inline struct jaVector3 jaVector3Scale(struct jaVector3 v, float scale)
{
	return (struct jaVector3){(v.x * scale), (v.y * scale), (v.z * scale)};
}


inline struct jaVector3 jaVector3Normalize(struct jaVector3 v)
{
	float length = jaVector3Length(v);
	return (struct jaVector3){(v.x / length), (v.y / length), (v.z / length)};
}


inline struct jaVector3 jaVector3Cross(struct jaVector3 a, struct jaVector3 b)
{
	return (struct jaVector3){(a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x)};
}


inline float jaVector3Length(struct jaVector3 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f));
}


inline float jaVector3Distance(struct jaVector3 a, struct jaVector3 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f) + powf(b.z - a.z, 2.0f));
}


inline float jaVector3Dot(struct jaVector3 a, struct jaVector3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


inline bool jaVector3Equals(struct jaVector3 a, struct jaVector3 b)
{
	if (a.x != b.x || a.y != b.y || a.z != b.z)
		return false;

	return true;
}


/*-----------------------------

 Vector4
-----------------------------*/
inline struct jaVector4 jaVector4Clean()
{
	return (struct jaVector4){0.0f, 0.0f, 0.0f, 0.0f};
}


inline struct jaVector4 jaVector4Set(float x, float y, float z, float u)
{
	return (struct jaVector4){x, y, z, u};
}


inline struct jaVector4 jaVector4Add(struct jaVector4 a, struct jaVector4 b)
{
	return (struct jaVector4){(a.x + b.x), (a.y + b.y), (a.z + b.z), (a.w + b.w)};
}


inline struct jaVector4 jaVector4Subtract(struct jaVector4 a, struct jaVector4 b)
{
	return (struct jaVector4){(a.x - b.x), (a.y - b.y), (a.z - b.z), (a.w - b.w)};
}


inline struct jaVector4 jaVector4Multiply(struct jaVector4 a, struct jaVector4 b)
{
	return (struct jaVector4){(a.x * b.x), (a.y * b.y), (a.z * b.z), (a.w * b.w)};
}


inline struct jaVector4 jaVector4Divide(struct jaVector4 a, struct jaVector4 b)
{
	return (struct jaVector4){(a.x / b.x), (a.y / b.y), (a.z / b.z), (a.w / b.w)};
}


inline struct jaVector4 jaVector4Invert(struct jaVector4 v)
{
	return (struct jaVector4){(-v.x), (-v.y), (-v.z), (-v.w)};
}


inline struct jaVector4 jaVector4Scale(struct jaVector4 v, float scale)
{
	return (struct jaVector4){(v.x * scale), (v.y * scale), (v.z * scale), (v.w * scale)};
}


inline struct jaVector4 jaVector4Normalize(struct jaVector4 v)
{
	float length = jaVector4Length(v);
	return (struct jaVector4){(v.x / length), (v.y / length), (v.z / length), (v.w / length)};
}


inline float jaVector4Length(struct jaVector4 v)
{
	return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f) + powf(v.w, 2.0f));
}


inline float jaVector4Distance(struct jaVector4 a, struct jaVector4 b)
{
	return sqrtf(powf(b.x - a.x, 2.0f) + powf(b.y - a.y, 2.0f) + powf(b.z - a.z, 2.0f) + powf(b.w - a.w, 2.0f));
}


inline float jaVector4Dot(struct jaVector4 a, struct jaVector4 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}


inline bool jaVector4Equals(struct jaVector4 a, struct jaVector4 b)
{
	if (a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w)
		return false;

	return true;
}
