/*-----------------------------

 [japan-matrix.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif


#ifndef JA_MATRIX_H
#define JA_MATRIX_H

#include "japan-vector.h"

struct jaMatrix4
{
	float e[4][4];
};

JA_EXPORT struct jaMatrix4 jaMatrix4Clean();
JA_EXPORT struct jaMatrix4 jaMatrix4Identity();

JA_EXPORT struct jaMatrix4 jaMatrix4Translate(struct jaVector3 origin);
JA_EXPORT struct jaMatrix4 jaMatrix4Orthographic(float left, float right, float bottom, float top, float near,
                                                 float far);
JA_EXPORT struct jaMatrix4 jaMatrix4Perspective(float y_fov, float aspect, float near, float far);
JA_EXPORT struct jaMatrix4 jaMatrix4LookAt(struct jaVector3 eye, struct jaVector3 center, struct jaVector3 up);

JA_EXPORT struct jaMatrix4 jaMatrix4Multiply(struct jaMatrix4 mat_a, struct jaMatrix4 mat_b);
JA_EXPORT struct jaMatrix4 jaMatrix4RotateX(struct jaMatrix4 mat, float angle);
JA_EXPORT struct jaMatrix4 jaMatrix4RotateY(struct jaMatrix4 mat, float angle);
JA_EXPORT struct jaMatrix4 jaMatrix4RotateZ(struct jaMatrix4 mat, float angle);
JA_EXPORT struct jaMatrix4 jaMatrix4Rotate(struct jaMatrix4 mat, struct jaVector3 v, float angle);

#endif
