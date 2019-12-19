/*-----------------------------

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

#include "japan-matrix.h"
#include <math.h>


inline struct jaMatrix4 jaMatrix4Clean()
{
	return (struct jaMatrix4){.e[0][0] = 0.0f,
	                          .e[0][1] = 0.0f,
	                          .e[0][2] = 0.0f,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = 0.0f,
	                          .e[1][1] = 0.0f,
	                          .e[1][2] = 0.0f,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = 0.0f,
	                          .e[2][1] = 0.0f,
	                          .e[2][2] = 0.0f,
	                          .e[2][3] = 0.0f,

	                          .e[3][0] = 0.0f,
	                          .e[3][1] = 0.0f,
	                          .e[3][2] = 0.0f,
	                          .e[3][3] = 0.0f};
}


inline struct jaMatrix4 jaMatrix4Identity()
{
	return (struct jaMatrix4){.e[0][0] = 1.0f,
	                          .e[0][1] = 0.0f,
	                          .e[0][2] = 0.0f,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = 0.0f,
	                          .e[1][1] = 1.0f,
	                          .e[1][2] = 0.0f,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = 0.0f,
	                          .e[2][1] = 0.0f,
	                          .e[2][2] = 1.0f,
	                          .e[2][3] = 0.0f,

	                          .e[3][0] = 0.0f,
	                          .e[3][1] = 0.0f,
	                          .e[3][2] = 0.0f,
	                          .e[3][3] = 1.0f};
}


inline struct jaMatrix4 jaMatrix4Translate(struct jaVector3 origin)
{
	return (struct jaMatrix4){.e[0][0] = 1.0f,
	                          .e[0][1] = 0.0f,
	                          .e[0][2] = 0.0f,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = 0.0f,
	                          .e[1][1] = 1.0f,
	                          .e[1][2] = 0.0f,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = 0.0f,
	                          .e[2][1] = 0.0f,
	                          .e[2][2] = 1.0f,
	                          .e[2][3] = 0.0f,

	                          .e[3][0] = origin.x,
	                          .e[3][1] = origin.y,
	                          .e[3][2] = origin.z,
	                          .e[3][3] = 1.0f};
}


inline struct jaMatrix4 jaMatrix4Orthographic(float left, float right, float bottom, float top, float near, float far)
{
	return (struct jaMatrix4){.e[0][0] = 2.0f / (right - left),
	                          .e[0][1] = 0.0f,
	                          .e[0][2] = 0.0f,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = 0.0f,
	                          .e[1][1] = 2.0f / (top - bottom),
	                          .e[1][2] = 0.0f,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = 0.0f,
	                          .e[2][1] = 0.0f,
	                          .e[2][2] = 2.0f / (far - near),
	                          .e[2][3] = 0.0f,

	                          .e[3][0] = -(right + left) / (right - left),
	                          .e[3][1] = -(top + bottom) / (top - bottom),
	                          .e[3][2] = -(far + near) / (far - near),
	                          .e[3][3] = 1.0f};
}


inline struct jaMatrix4 jaMatrix4Perspective(float y_fov, float aspect, float near, float far)
{
	float a = 1.0f / tanf(y_fov / 2.0f);

	return (struct jaMatrix4){.e[0][0] = a / aspect,
	                          .e[0][1] = 0.0f,
	                          .e[0][2] = 0.0f,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = 0.0f,
	                          .e[1][1] = a,
	                          .e[1][2] = 0.0f,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = 0.0f,
	                          .e[2][1] = 0.0f,
	                          .e[2][2] = -((far + near) / (far - near)),
	                          .e[2][3] = -1.0f,

	                          .e[3][0] = 0.0f,
	                          .e[3][1] = 0.0f,
	                          .e[3][2] = -((2.0f * far * near) / (far - near)),
	                          .e[3][3] = 0.0f};
}


inline struct jaMatrix4 jaMatrix4LookAt(struct jaVector3 eye, struct jaVector3 center, struct jaVector3 up)
{
	struct jaVector3 f = jaVector3Subtract(center, eye);
	f = jaVector3Normalize(f);

	struct jaVector3 s = jaVector3Cross(f, up);
	s = jaVector3Normalize(s);

	struct jaVector3 u = jaVector3Cross(s, f);

	return (struct jaMatrix4){.e[0][0] = s.x,
	                          .e[0][1] = u.x,
	                          .e[0][2] = -f.x,
	                          .e[0][3] = 0.0f,

	                          .e[1][0] = s.y,
	                          .e[1][1] = u.y,
	                          .e[1][2] = -f.y,
	                          .e[1][3] = 0.0f,

	                          .e[2][0] = s.z,
	                          .e[2][1] = u.z,
	                          .e[2][2] = -f.z,
	                          .e[2][3] = 0.0f,

	                          .e[3][0] = (s.x * (-eye.x) + s.y * (-eye.y) + s.z * (-eye.z)),
	                          .e[3][1] = (u.x * (-eye.x) + u.y * (-eye.y) + u.z * (-eye.z)),
	                          .e[3][2] = ((-f.x) * (-eye.x) + (-f.y) * (-eye.y) + (-f.z) * (-eye.z)),
	                          .e[3][3] = 1.0f};
}


inline struct jaMatrix4 jaMatrix4Multiply(struct jaMatrix4 a, struct jaMatrix4 b)
{
	return (struct jaMatrix4){
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


inline struct jaMatrix4 jaMatrix4RotateX(struct jaMatrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrix4 r = jaMatrix4Identity();

	// 1.0f  0.0f  0.0f  0.0f
	// 0.0f  c     s     0.0f
	// 0.0f  -s    c     0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[1][1] = c;
	r.e[1][2] = s;
	r.e[2][1] = -s;
	r.e[2][2] = c;

	return jaMatrix4Multiply(mat, r);
}


inline struct jaMatrix4 jaMatrix4RotateY(struct jaMatrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrix4 r = jaMatrix4Identity();

	// c     0.0f  s     0.0f
	// 0.0f  1.0f  0.0f  0.0f
	// -s    0.0f  c     0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[0][0] = c;
	r.e[0][2] = s;
	r.e[2][0] = -s;
	r.e[2][2] = c;

	return jaMatrix4Multiply(mat, r);
}


inline struct jaMatrix4 jaMatrix4RotateZ(struct jaMatrix4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrix4 r = jaMatrix4Identity();

	// c     s     0.0f  0.0f
	// -s    c     0.0f  0.0f
	// 0.0f  0.0f  1.0f  0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[0][0] = c;
	r.e[0][1] = s;
	r.e[1][0] = -s;
	r.e[1][1] = c;

	return jaMatrix4Multiply(mat, r);
}


inline struct jaMatrix4 jaMatrix4Rotate(struct jaMatrix4 mat, struct jaVector3 v, float angle)
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrix4 r;

	r.e[0][0] = (v.x * v.x) * (1.0f - c) + c;
	r.e[0][1] = (v.x * v.y) * (1.0f - c) - v.z * s;
	r.e[0][2] = (v.x * v.z) * (1.0f - c) + v.y * s;
	r.e[0][3] = 0.0f;

	r.e[1][0] = (v.y * v.x) * (1.0f - c) + v.z * s;
	r.e[1][1] = (v.y * v.y) * (1.0f - c) + c;
	r.e[1][2] = (v.y * v.z) * (1.0f - c) - v.x * s;
	r.e[1][3] = 0.0f;

	r.e[2][0] = (v.z * v.x) * (1.0f - c) - v.y * s;
	r.e[2][1] = (v.z * v.y) * (1.0f - c) + v.x * s;
	r.e[2][2] = (v.z * v.z) * (1.0f - c) + c;
	r.e[2][3] = 0.0f;

	r.e[3][0] = 0.0f;
	r.e[3][1] = 0.0f;
	r.e[3][2] = 0.0f;
	r.e[3][3] = 1.0f;

	return jaMatrix4Multiply(mat, r);
}
