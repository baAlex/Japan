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
-----------------------------*/

#include "matrix.h"
#include <math.h>

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


/*-----------------------------

 Matrix4Clean()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Clean()
{
	return (struct Matrix4){.e[0][0] = 0.0,
	                        .e[0][1] = 0.0,
	                        .e[0][2] = 0.0,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = 0.0,
	                        .e[1][1] = 0.0,
	                        .e[1][2] = 0.0,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = 0.0,
	                        .e[2][1] = 0.0,
	                        .e[2][2] = 0.0,
	                        .e[2][3] = 0.0,

	                        .e[3][0] = 0.0,
	                        .e[3][1] = 0.0,
	                        .e[3][2] = 0.0,
	                        .e[3][3] = 0.0};
}


/*-----------------------------

 Matrix4Identity()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Identity()
{
	return (struct Matrix4){.e[0][0] = 1.0,
	                        .e[0][1] = 0.0,
	                        .e[0][2] = 0.0,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = 0.0,
	                        .e[1][1] = 1.0,
	                        .e[1][2] = 0.0,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = 0.0,
	                        .e[2][1] = 0.0,
	                        .e[2][2] = 1.0,
	                        .e[2][3] = 0.0,

	                        .e[3][0] = 0.0,
	                        .e[3][1] = 0.0,
	                        .e[3][2] = 0.0,
	                        .e[3][3] = 1.0};
}


/*-----------------------------

 Matrix4Translate()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Translate(struct Vector3 origin)
{
	return (struct Matrix4){.e[0][0] = 1.0,
	                        .e[0][1] = 0.0,
	                        .e[0][2] = 0.0,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = 0.0,
	                        .e[1][1] = 1.0,
	                        .e[1][2] = 0.0,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = 0.0,
	                        .e[2][1] = 0.0,
	                        .e[2][2] = 1.0,
	                        .e[2][3] = 0.0,

	                        .e[3][0] = origin.x,
	                        .e[3][1] = origin.y,
	                        .e[3][2] = origin.z,
	                        .e[3][3] = 1.0};
}


/*-----------------------------

 Matrix4Orthographic()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Orthographic(float left, float right, float bottom, float top, float near,
                                                 float far)
{
	return (struct Matrix4){.e[0][0] = 2.0 / (right - left),
	                        .e[0][1] = 0.0,
	                        .e[0][2] = 0.0,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = 0.0,
	                        .e[1][1] = 2.0 / (top - bottom),
	                        .e[1][2] = 0.0,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = 0.0,
	                        .e[2][1] = 0.0,
	                        .e[2][2] = 2.0 / (far - near),
	                        .e[2][3] = 0.0,

	                        .e[3][0] = -(right + left) / (right - left),
	                        .e[3][1] = -(top + bottom) / (top - bottom),
	                        .e[3][2] = -(far + near) / (far - near),
	                        .e[3][3] = 1.0};
}


/*-----------------------------

 Matrix4Perspective()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Perspective(float y_fov, float aspect, float near, float far)
{
	float a = 1.0 / tanf(y_fov / 2.0);

	return (struct Matrix4){.e[0][0] = a / aspect,
	                        .e[0][1] = 0.0,
	                        .e[0][2] = 0.0,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = 0.0,
	                        .e[1][1] = a,
	                        .e[1][2] = 0.0,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = 0.0,
	                        .e[2][1] = 0.0,
	                        .e[2][2] = -((far + near) / (far - near)),
	                        .e[2][3] = -1.0,

	                        .e[3][0] = 0.0,
	                        .e[3][1] = 0.0,
	                        .e[3][2] = -((2.0 * far * near) / (far - near)),
	                        .e[3][3] = 0.0};
}


/*-----------------------------

 Matrix4LookAt()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4LookAt(struct Vector3 eye, struct Vector3 center, struct Vector3 up)
{
	struct Vector3 f = Vector3Subtract(center, eye);
	f = Vector3Normalize(f);

	struct Vector3 s = Vector3Cross(f, up);
	s = Vector3Normalize(s);

	struct Vector3 u = Vector3Cross(s, f);

	return (struct Matrix4){.e[0][0] = s.x,
	                        .e[0][1] = u.x,
	                        .e[0][2] = -f.x,
	                        .e[0][3] = 0.0,

	                        .e[1][0] = s.y,
	                        .e[1][1] = u.y,
	                        .e[1][2] = -f.y,
	                        .e[1][3] = 0.0,

	                        .e[2][0] = s.z,
	                        .e[2][1] = u.z,
	                        .e[2][2] = -f.z,
	                        .e[2][3] = 0.0,

	                        .e[3][0] = (s.x * (-eye.x) + s.y * (-eye.y) + s.z * (-eye.z)),
	                        .e[3][1] = (u.x * (-eye.x) + u.y * (-eye.y) + u.z * (-eye.z)),
	                        .e[3][2] = ((-f.x) * (-eye.x) + (-f.y) * (-eye.y) + (-f.z) * (-eye.z)),
	                        .e[3][3] = 1.0};
}


/*-----------------------------

 Matrix4Multiply()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4Multiply(struct Matrix4 a, struct Matrix4 b)
{
	return (struct Matrix4){
	    .e[0][0] = a.e[0][0] * b.e[0][0] + a.e[1][0] * b.e[0][1] + a.e[2][0] * b.e[0][2] + a.e[3][0] * b.e[0][3],
	    .e[0][1] = a.e[0][1] * b.e[0][0] + a.e[1][1] * b.e[0][1] + a.e[2][1] * b.e[0][2] + a.e[3][1] * b.e[0][3],
	    .e[0][2] = a.e[0][2] * b.e[0][0] + a.e[1][2] * b.e[0][1] + a.e[2][2] * b.e[0][2] + a.e[3][2] * b.e[0][3],
	    .e[0][3] = a.e[0][3] * b.e[0][0] + a.e[1][3] * b.e[0][1] + a.e[2][3] * b.e[0][2] + a.e[3][3] * b.e[0][3],

	    .e[1][0] = a.e[0][0] * b.e[1][0] + a.e[1][0] * b.e[1][1] + a.e[2][0] * b.e[1][2] + a.e[3][0] * b.e[1][3],
	    .e[1][1] = a.e[0][1] * b.e[1][0] + a.e[1][1] * b.e[1][1] + a.e[2][1] * b.e[1][2] + a.e[3][1] * b.e[1][3],
	    .e[1][2] = a.e[0][2] * b.e[1][0] + a.e[1][2] * b.e[1][1] + a.e[2][2] * b.e[1][2] + a.e[3][2] * b.e[1][3],
	    .e[1][3] = a.e[0][3] * b.e[1][0] + a.e[1][3] * b.e[1][1] + a.e[2][3] * b.e[1][2] + a.e[3][3] * b.e[1][3],

	    .e[2][0] = a.e[0][0] * b.e[2][0] + a.e[1][0] * b.e[2][1] + a.e[2][0] * b.e[2][2] + a.e[3][0] * b.e[2][3],
	    .e[2][1] = a.e[0][1] * b.e[2][0] + a.e[1][1] * b.e[2][1] + a.e[2][1] * b.e[2][2] + a.e[3][1] * b.e[2][3],
	    .e[2][2] = a.e[0][2] * b.e[2][0] + a.e[1][2] * b.e[2][1] + a.e[2][2] * b.e[2][2] + a.e[3][2] * b.e[2][3],
	    .e[2][3] = a.e[0][3] * b.e[2][0] + a.e[1][3] * b.e[2][1] + a.e[2][3] * b.e[2][2] + a.e[3][3] * b.e[2][3],

	    .e[3][0] = a.e[0][0] * b.e[3][0] + a.e[1][0] * b.e[3][1] + a.e[2][0] * b.e[3][2] + a.e[3][0] * b.e[3][3],
	    .e[3][1] = a.e[0][1] * b.e[3][0] + a.e[1][1] * b.e[3][1] + a.e[2][1] * b.e[3][2] + a.e[3][1] * b.e[3][3],
	    .e[3][2] = a.e[0][2] * b.e[3][0] + a.e[1][2] * b.e[3][1] + a.e[2][2] * b.e[3][2] + a.e[3][2] * b.e[3][3],
	    .e[3][3] = a.e[0][3] * b.e[3][0] + a.e[1][3] * b.e[3][1] + a.e[2][3] * b.e[3][2] + a.e[3][3] * b.e[3][3]};
}


/*-----------------------------

 Matrix4RotateX()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4RotateX(struct Matrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct Matrix4 r = Matrix4Identity();

	// 1.0  0.0  0.0  0.0
	// 0.0  c    s    0.0
	// 0.0  -s   c    0.0
	// 0.0  0.0  0.0  1.0

	r.e[1][1] = c;
	r.e[1][2] = s;
	r.e[2][1] = -s;
	r.e[2][2] = c;

	return Matrix4Multiply(mat, r);
}


/*-----------------------------

 Matrix4RotateY()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4RotateY(struct Matrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct Matrix4 r = Matrix4Identity();

	// c    0.0  s    0.0
	// 0.0  1.0  0.0  0.0
	// -s   0.0  c    0.0
	// 0.0  0.0  0.0  1.0

	r.e[0][0] = c;
	r.e[0][2] = s;
	r.e[2][0] = -s;
	r.e[2][2] = c;

	return Matrix4Multiply(mat, r);
}


/*-----------------------------

 Matrix4RotateZ()
-----------------------------*/
EXPORT inline struct Matrix4 Matrix4RotateZ(struct Matrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct Matrix4 r = Matrix4Identity();

	// c    s    0.0  0.0
	// -s   c    0.0  0.0
	// 0.0  0.0  1.0  0.0
	// 0.0  0.0  0.0  1.0

	r.e[0][0] = c;
	r.e[0][1] = s;
	r.e[1][0] = -s;
	r.e[1][1] = c;

	return Matrix4Multiply(mat, r);
}


/*-----------------------------

 Matrix4Rotate()
-----------------------------*/
EXPORT inline void Matrix4Rotate(struct Matrix4 mat, struct Vector3 v, float angle)
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
	float s = sinf(angle);
	float c = cosf(angle);

	struct Matrix4 r;

	r.e[0][0] = (v.x * v.x) * (1.0 - c) + c;
	r.e[0][1] = (v.x * v.y) * (1.0 - c) - v.z * s;
	r.e[0][2] = (v.x * v.z) * (1.0 - c) + v.y * s;
	r.e[0][3] = 0.0;

	r.e[1][0] = (v.y * v.x) * (1.0 - c) + v.z * s;
	r.e[1][1] = (v.y * v.y) * (1.0 - c) + c;
	r.e[1][2] = (v.y * v.z) * (1.0 - c) - v.x * s;
	r.e[1][3] = 0.0;

	r.e[2][0] = (v.z * v.x) * (1.0 - c) - v.y * s;
	r.e[2][1] = (v.z * v.y) * (1.0 - c) + v.x * s;
	r.e[2][2] = (v.z * v.z) * (1.0 - c) + c;
	r.e[2][3] = 0.0;

	r.e[3][0] = 0.0;
	r.e[3][1] = 0.0;
	r.e[3][2] = 0.0;
	r.e[3][3] = 1.0;

	return Matrix4Multiply(mat, r);
}
