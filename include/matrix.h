/*-----------------------------

 [matrix.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef MATRIX_H
#define MATRIX_H

	#include "vector.h"

	struct Matrix4
	{
		float e[16];
	};

	struct Matrix4 Matrix4Clean();
	struct Matrix4 Matrix4Identity();

	struct Matrix4 Matrix4Translate(struct Vector3 origin);
	struct Matrix4 Matrix4Orthographic(float left, float right, float bottom, float top, float near, float far);
	struct Matrix4 Matrix4Perspective(float y_fov, float aspect, float near, float far);
	struct Matrix4 Matrix4LookAt(struct Vector3 eye, struct Vector3 center, struct Vector3 up);

	struct Matrix4* Matrix4Multiply(const struct Matrix4* mat_a, const struct Matrix4* mat_b, struct Matrix4* out);

#endif
