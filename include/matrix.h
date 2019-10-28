/*-----------------------------

 [matrix.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.2"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 2
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
#endif

#ifndef MATRIX_H
#define MATRIX_H

	#include "vector.h"

	struct Matrix4
	{
		float e[4][4];
	};

	DLL_EXP struct Matrix4 Matrix4Clean();
	DLL_EXP struct Matrix4 Matrix4Identity();

	DLL_EXP struct Matrix4 Matrix4Translate(struct Vector3 origin);
	DLL_EXP struct Matrix4 Matrix4Orthographic(float left, float right, float bottom, float top, float near, float far);
	DLL_EXP struct Matrix4 Matrix4Perspective(float y_fov, float aspect, float near, float far);
	DLL_EXP struct Matrix4 Matrix4LookAt(struct Vector3 eye, struct Vector3 center, struct Vector3 up);

	DLL_EXP struct Matrix4 Matrix4Multiply(struct Matrix4 mat_a, const struct Matrix4 mat_b);
	DLL_EXP struct Matrix4 Matrix4RotateX(struct Matrix4 mat, float angle);
	DLL_EXP struct Matrix4 Matrix4RotateY(struct Matrix4 mat, float angle);
	DLL_EXP struct Matrix4 Matrix4RotateZ(struct Matrix4 mat, float angle);
	DLL_EXP struct Matrix4 Matrix4Rotate(struct Matrix4 mat, struct Vector3 v, float angle);

#endif
