/*-----------------------------

 [japan-matrix.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_MATRIX_H
#define JAPAN_MATRIX_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include "japan-vector.h"

struct jaMatrixF4
{
	float e[4][4];
};

JA_EXPORT struct jaMatrixF4 jaMatrixF4Clean();
JA_EXPORT struct jaMatrixF4 jaMatrixF4Identity();

JA_EXPORT struct jaMatrixF4 jaMatrixTranslationF4(struct jaVectorF3 origin);
JA_EXPORT struct jaMatrixF4 jaMatrixOrthographicF4(float left, float right, float bottom, float top, float near, float far);
JA_EXPORT struct jaMatrixF4 jaMatrixPerspectiveF4(float y_fov, float aspect, float near, float far);
JA_EXPORT struct jaMatrixF4 jaMatrixLookAtF4(struct jaVectorF3 eye, struct jaVectorF3 center, struct jaVectorF3 up);

JA_EXPORT struct jaMatrixF4 jaMatrixMultiplyF4(struct jaMatrixF4 mat_a, struct jaMatrixF4 mat_b);
JA_EXPORT struct jaMatrixF4 jaMatrixRotateXF4(struct jaMatrixF4 mat, float angle);
JA_EXPORT struct jaMatrixF4 jaMatrixRotateYF4(struct jaMatrixF4 mat, float angle);
JA_EXPORT struct jaMatrixF4 jaMatrixRotateZF4(struct jaMatrixF4 mat, float angle);
JA_EXPORT struct jaMatrixF4 jaMatrixRotateF4(struct jaMatrixF4 mat, struct jaVectorF3 v, float angle);

JA_EXPORT struct jaMatrixF4 jaMatrixScaleAnsioF4(struct jaMatrixF4 mat, struct jaVectorF3 factor);
JA_EXPORT struct jaMatrixF4 jaMatrixScaleF4(struct jaMatrixF4 mat, float factor);

JA_EXPORT struct jaMatrixF4 jaMatrixInvertF4(struct jaMatrixF4 mat);

#endif
