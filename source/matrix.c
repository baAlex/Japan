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

linmath.h -- A small library for linear math as required for computer graphics
https://github.com/datenwolf/linmath.h

Copyright (C) 2013 Wolfgang 'datenwolf' Draxinger <code@datenwolf.net>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE F*CK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE F*CK YOU WANT TO.

-------------------------------

 [matrix.c]
 - Alexander Brandt 2019

 Check the linmath project, what
 follows is just a caricature.
-----------------------------*/

#include "matrix.h"
#include <math.h>

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


static inline struct Vector3 sVector3Cross(struct Vector3 const a, struct Vector3 const b)
{
	return (struct Vector3){(a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x)};
}


EXPORT struct Matrix4 Matrix4Clean()
{
	return (struct Matrix4){.e[0] = 0.0,
							.e[1] = 0.0,
							.e[2] = 0.0,
							.e[3] = 0.0,

							.e[4] = 0.0,
							.e[5] = 0.0,
							.e[6] = 0.0,
							.e[7] = 0.0,

							.e[8] = 0.0,
							.e[9] = 0.0,
							.e[10] = 0.0,
							.e[11] = 0.0,

							.e[12] = 0.0,
							.e[13] = 0.0,
							.e[14] = 0.0,
							.e[15] = 0.0};
}


EXPORT struct Matrix4 Matrix4Identity()
{
	return (struct Matrix4){.e[0] = 1.0,
							.e[1] = 0.0,
							.e[2] = 0.0,
							.e[3] = 0.0,

							.e[4] = 0.0,
							.e[5] = 1.0,
							.e[6] = 0.0,
							.e[7] = 0.0,

							.e[8] = 0.0,
							.e[9] = 0.0,
							.e[10] = 1.0,
							.e[11] = 0.0,

							.e[12] = 0.0,
							.e[13] = 0.0,
							.e[14] = 0.0,
							.e[15] = 1.0};
}


EXPORT struct Matrix4 Matrix4Translate(struct Vector3 origin)
{
	return (struct Matrix4){.e[0] = 1.0,
							.e[1] = 0.0,
							.e[2] = 0.0,
							.e[3] = 0.0,

							.e[4] = 0.0,
							.e[5] = 1.0,
							.e[6] = 0.0,
							.e[7] = 0.0,

							.e[8] = 0.0,
							.e[9] = 0.0,
							.e[10] = 1.0,
							.e[11] = 0.0,

							.e[12] = origin.x,
							.e[13] = origin.y,
							.e[14] = origin.z,
							.e[15] = 1.0};
}


EXPORT struct Matrix4 Matrix4Orthographic(float left, float right, float bottom, float top, float near, float far)
{
	return (struct Matrix4){.e[0] = 2.0 / (right - left),
							.e[1] = 0.0,
							.e[2] = 0.0,
							.e[3] = 0.0,

							.e[4] = 0.0,
							.e[5] = 2.0 / (top - bottom),
							.e[6] = 0.0,
							.e[7] = 0.0,

							.e[8] = 0.0,
							.e[9] = 0.0,
							.e[10] = 2.0 / (far - near),
							.e[11] = 0.0,

							.e[12] = -(right + left) / (right - left),
							.e[13] = -(top + bottom) / (top - bottom),
							.e[14] = -(far + near) / (far - near),
							.e[15] = 1.0};
}


EXPORT struct Matrix4 Matrix4Perspective(float y_fov, float aspect, float near, float far)
{
	float const a = 1.f / tanf(y_fov / 2.f);

	return (struct Matrix4){.e[0] = a / aspect,
							.e[1] = 0.0,
							.e[2] = 0.0,
							.e[3] = 0.0,

							.e[4] = 0.0,
							.e[5] = a,
							.e[6] = 0.0,
							.e[7] = 0.0,

							.e[8] = 0.0,
							.e[9] = 0.0,
							.e[10] = -((far + near) / (far - near)),
							.e[11] = -1.0,

							.e[12] = 0.0,
							.e[13] = 0.0,
							.e[14] = -((2.0 * far * near) / (far - near)),
							.e[15] = 0.0};
}


EXPORT struct Matrix4 Matrix4LookAt(struct Vector3 eye, struct Vector3 center, struct Vector3 up)
{
	struct Vector3 f = Vector3Subtract(center, eye);
	f = Vector3Normalize(f);

	struct Vector3 s = sVector3Cross(f, up);
	s = Vector3Normalize(s);

	struct Vector3 u = sVector3Cross(s, f);

	return (struct Matrix4){.e[0] = s.x,
							.e[1] = u.x,
							.e[2] = -f.x,
							.e[3] = 0.0,

							.e[4] = s.y,
							.e[5] = u.y,
							.e[6] = -f.y,
							.e[7] = 0.0,

							.e[8] = s.z,
							.e[9] = u.z,
							.e[10] = -f.z,
							.e[11] = 0.0,

							.e[12] = (s.x * (-eye.x) + s.y * (-eye.y) + s.z * (-eye.z)),
							.e[13] = (u.x * (-eye.x) + u.y * (-eye.y) + u.z * (-eye.z)),
							.e[14] = ((-f.x) * (-eye.x) + (-f.y) * (-eye.y) + (-f.z) * (-eye.z)),
							.e[15] = 1.0};
}


EXPORT struct Matrix4* Matrix4Multiply(const struct Matrix4* a, const struct Matrix4* b, struct Matrix4* out)
{
	out->e[0] = a->e[0] * b->e[0] + a->e[4] * b->e[1] + a->e[8] * b->e[2] + a->e[12] * b->e[3];
	out->e[1] = a->e[1] * b->e[0] + a->e[5] * b->e[1] + a->e[9] * b->e[2] + a->e[13] * b->e[3];
	out->e[2] = a->e[2] * b->e[0] + a->e[6] * b->e[1] + a->e[10] * b->e[2] + a->e[14] * b->e[3];
	out->e[3] = a->e[3] * b->e[0] + a->e[7] * b->e[1] + a->e[11] * b->e[2] + a->e[15] * b->e[3];

	out->e[4] = a->e[0] * b->e[4] + a->e[4] * b->e[5] + a->e[8] * b->e[6] + a->e[12] * b->e[7];
	out->e[5] = a->e[1] * b->e[4] + a->e[5] * b->e[5] + a->e[9] * b->e[6] + a->e[13] * b->e[7];
	out->e[6] = a->e[2] * b->e[4] + a->e[6] * b->e[5] + a->e[10] * b->e[6] + a->e[14] * b->e[7];
	out->e[7] = a->e[3] * b->e[4] + a->e[7] * b->e[5] + a->e[11] * b->e[6] + a->e[15] * b->e[7];

	out->e[8] = a->e[0] * b->e[8] + a->e[4] * b->e[9] + a->e[8] * b->e[10] + a->e[12] * b->e[11];
	out->e[9] = a->e[1] * b->e[8] + a->e[5] * b->e[9] + a->e[9] * b->e[10] + a->e[13] * b->e[11];
	out->e[10] = a->e[2] * b->e[8] + a->e[6] * b->e[9] + a->e[10] * b->e[10] + a->e[14] * b->e[11];
	out->e[11] = a->e[3] * b->e[8] + a->e[7] * b->e[9] + a->e[11] * b->e[10] + a->e[15] * b->e[11];

	out->e[12] = a->e[0] * b->e[12] + a->e[4] * b->e[13] + a->e[8] * b->e[14] + a->e[12] * b->e[15];
	out->e[13] = a->e[1] * b->e[12] + a->e[5] * b->e[13] + a->e[9] * b->e[14] + a->e[13] * b->e[15];
	out->e[14] = a->e[2] * b->e[12] + a->e[6] * b->e[13] + a->e[10] * b->e[14] + a->e[14] * b->e[15];
	out->e[15] = a->e[3] * b->e[12] + a->e[7] * b->e[13] + a->e[11] * b->e[14] + a->e[15] * b->e[15];

	return out;
}
