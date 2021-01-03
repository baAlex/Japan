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
 - Alexander Brandt 2019-2020
-----------------------------*/

#include "japan-matrix.h"
#include <math.h>


inline struct jaMatrixF4 jaMatrixF4Clean()
{
	return (struct jaMatrixF4){.e[0][0] = 0.0f,
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


inline struct jaMatrixF4 jaMatrixF4Identity()
{
	return (struct jaMatrixF4){.e[0][0] = 1.0f,
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


inline struct jaMatrixF4 jaMatrixTranslationF4(struct jaVectorF3 origin)
{
	return (struct jaMatrixF4){.e[0][0] = 1.0f,
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


inline struct jaMatrixF4 jaMatrixOrthographicF4(float left, float right, float bottom, float top, float near, float far)
{
	return (struct jaMatrixF4){.e[0][0] = 2.0f / (right - left),
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


inline struct jaMatrixF4 jaMatrixPerspectiveF4(float y_fov, float aspect, float near, float far)
{
	float a = 1.0f / tanf(y_fov / 2.0f);

	return (struct jaMatrixF4){.e[0][0] = a / aspect,
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


inline struct jaMatrixF4 jaMatrixLookAtF4(struct jaVectorF3 eye, struct jaVectorF3 center, struct jaVectorF3 up)
{
	struct jaVectorF3 f = jaVectorSubtractF3(center, eye);
	f = jaVectorNormalizeF3(f);

	struct jaVectorF3 s = jaVectorCrossF3(f, up);
	s = jaVectorNormalizeF3(s);

	struct jaVectorF3 u = jaVectorCrossF3(s, f);

	return (struct jaMatrixF4){.e[0][0] = s.x,
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


inline struct jaMatrixF4 jaMatrixMultiplyF4(struct jaMatrixF4 a, struct jaMatrixF4 b)
{
	return (struct jaMatrixF4){
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


inline struct jaMatrixF4 jaMatrixRotateXF4(struct jaMatrixF4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrixF4 r = jaMatrixF4Identity();

	// 1.0f  0.0f  0.0f  0.0f
	// 0.0f  c     s     0.0f
	// 0.0f  -s    c     0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[1][1] = c;
	r.e[1][2] = s;
	r.e[2][1] = -s;
	r.e[2][2] = c;

	return jaMatrixMultiplyF4(mat, r);
}


inline struct jaMatrixF4 jaMatrixRotateYF4(struct jaMatrixF4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrixF4 r = jaMatrixF4Identity();

	// c     0.0f  s     0.0f
	// 0.0f  1.0f  0.0f  0.0f
	// -s    0.0f  c     0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[0][0] = c;
	r.e[0][2] = s;
	r.e[2][0] = -s;
	r.e[2][2] = c;

	return jaMatrixMultiplyF4(mat, r);
}


inline struct jaMatrixF4 jaMatrixRotateZF4(struct jaMatrixF4 mat, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrixF4 r = jaMatrixF4Identity();

	// c     s     0.0f  0.0f
	// -s    c     0.0f  0.0f
	// 0.0f  0.0f  1.0f  0.0f
	// 0.0f  0.0f  0.0f  1.0f

	r.e[0][0] = c;
	r.e[0][1] = s;
	r.e[1][0] = -s;
	r.e[1][1] = c;

	return jaMatrixMultiplyF4(mat, r);
}


inline struct jaMatrixF4 jaMatrixRotateF4(struct jaMatrixF4 mat, struct jaVectorF3 v, float angle)
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
	float s = sinf(angle);
	float c = cosf(angle);

	struct jaMatrixF4 r;

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

	return jaMatrixMultiplyF4(mat, r);
}


inline struct jaMatrixF4 jaMatrixScaleAnsioF4(struct jaMatrixF4 mat, struct jaVectorF3 f)
{
	return (struct jaMatrixF4){.e[0][0] = mat.e[0][0] * f.x,
	                          .e[0][1] = mat.e[0][1] * f.x,
	                          .e[0][2] = mat.e[0][2] * f.x,
	                          .e[0][3] = mat.e[0][3] * f.x,

	                          .e[1][0] = mat.e[1][0] * f.y,
	                          .e[1][1] = mat.e[1][1] * f.y,
	                          .e[1][2] = mat.e[1][2] * f.y,
	                          .e[1][3] = mat.e[1][3] * f.y,

	                          .e[2][0] = mat.e[2][0] * f.z,
	                          .e[2][1] = mat.e[2][1] * f.z,
	                          .e[2][2] = mat.e[2][2] * f.z,
	                          .e[2][3] = mat.e[2][3] * f.z,

	                          .e[3][0] = mat.e[3][0],
	                          .e[3][1] = mat.e[3][1],
	                          .e[3][2] = mat.e[3][2],
	                          .e[3][3] = mat.e[3][3]};
}


inline struct jaMatrixF4 jaMatrixScaleF4(struct jaMatrixF4 mat, float f)
{
	return (struct jaMatrixF4){.e[0][0] = mat.e[0][0] * f,
	                          .e[0][1] = mat.e[0][1] * f,
	                          .e[0][2] = mat.e[0][2] * f,
	                          .e[0][3] = mat.e[0][3] * f,

	                          .e[1][0] = mat.e[1][0] * f,
	                          .e[1][1] = mat.e[1][1] * f,
	                          .e[1][2] = mat.e[1][2] * f,
	                          .e[1][3] = mat.e[1][3] * f,

	                          .e[2][0] = mat.e[2][0] * f,
	                          .e[2][1] = mat.e[2][1] * f,
	                          .e[2][2] = mat.e[2][2] * f,
	                          .e[2][3] = mat.e[2][3] * f,

	                          .e[3][0] = mat.e[3][0] * f,
	                          .e[3][1] = mat.e[3][1] * f,
	                          .e[3][2] = mat.e[3][2] * f,
	                          .e[3][3] = mat.e[3][3] * f};
}


struct jaMatrixF4 jaMatrixInvertF4(struct jaMatrixF4 mat)
{
	float s[6];
	float c[6];

	s[0] = mat.e[0][0] * mat.e[1][1] - mat.e[1][0] * mat.e[0][1];
	s[1] = mat.e[0][0] * mat.e[1][2] - mat.e[1][0] * mat.e[0][2];
	s[2] = mat.e[0][0] * mat.e[1][3] - mat.e[1][0] * mat.e[0][3];
	s[3] = mat.e[0][1] * mat.e[1][2] - mat.e[1][1] * mat.e[0][2];
	s[4] = mat.e[0][1] * mat.e[1][3] - mat.e[1][1] * mat.e[0][3];
	s[5] = mat.e[0][2] * mat.e[1][3] - mat.e[1][2] * mat.e[0][3];

	c[0] = mat.e[2][0] * mat.e[3][1] - mat.e[3][0] * mat.e[2][1];
	c[1] = mat.e[2][0] * mat.e[3][2] - mat.e[3][0] * mat.e[2][2];
	c[2] = mat.e[2][0] * mat.e[3][3] - mat.e[3][0] * mat.e[2][3];
	c[3] = mat.e[2][1] * mat.e[3][2] - mat.e[3][1] * mat.e[2][2];
	c[4] = mat.e[2][1] * mat.e[3][3] - mat.e[3][1] * mat.e[2][3];
	c[5] = mat.e[2][2] * mat.e[3][3] - mat.e[3][2] * mat.e[2][3];

	// Assumes it is invertible
	float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

	return (struct jaMatrixF4){.e[0][0] = (+mat.e[1][1] * c[5] - mat.e[1][2] * c[4] + mat.e[1][3] * c[3]) * idet,
	                          .e[0][1] = (-mat.e[0][1] * c[5] + mat.e[0][2] * c[4] - mat.e[0][3] * c[3]) * idet,
	                          .e[0][2] = (+mat.e[3][1] * s[5] - mat.e[3][2] * s[4] + mat.e[3][3] * s[3]) * idet,
	                          .e[0][3] = (-mat.e[2][1] * s[5] + mat.e[2][2] * s[4] - mat.e[2][3] * s[3]) * idet,

	                          .e[1][0] = (-mat.e[1][0] * c[5] + mat.e[1][2] * c[2] - mat.e[1][3] * c[1]) * idet,
	                          .e[1][1] = (+mat.e[0][0] * c[5] - mat.e[0][2] * c[2] + mat.e[0][3] * c[1]) * idet,
	                          .e[1][2] = (-mat.e[3][0] * s[5] + mat.e[3][2] * s[2] - mat.e[3][3] * s[1]) * idet,
	                          .e[1][3] = (+mat.e[2][0] * s[5] - mat.e[2][2] * s[2] + mat.e[2][3] * s[1]) * idet,

	                          .e[2][0] = (+mat.e[1][0] * c[4] - mat.e[1][1] * c[2] + mat.e[1][3] * c[0]) * idet,
	                          .e[2][1] = (-mat.e[0][0] * c[4] + mat.e[0][1] * c[2] - mat.e[0][3] * c[0]) * idet,
	                          .e[2][2] = (+mat.e[3][0] * s[4] - mat.e[3][1] * s[2] + mat.e[3][3] * s[0]) * idet,
	                          .e[2][3] = (-mat.e[2][0] * s[4] + mat.e[2][1] * s[2] - mat.e[2][3] * s[0]) * idet,

	                          .e[3][0] = (-mat.e[1][0] * c[3] + mat.e[1][1] * c[1] - mat.e[1][2] * c[0]) * idet,
	                          .e[3][1] = (+mat.e[0][0] * c[3] - mat.e[0][1] * c[1] + mat.e[0][2] * c[0]) * idet,
	                          .e[3][2] = (-mat.e[3][0] * s[3] + mat.e[3][1] * s[1] - mat.e[3][2] * s[0]) * idet,
	                          .e[3][3] = (+mat.e[2][0] * s[3] - mat.e[2][1] * s[1] + mat.e[2][2] * s[0]) * idet};
}
