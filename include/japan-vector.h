/*-----------------------------

 [japan-vector.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_VECTOR_H
#define JAPAN_VECTOR_H

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

#include <stdbool.h>

struct jaVector2i
{
	int x;
	int y;
};

struct jaVector3i
{
	int x;
	int y;
	int z;
};

struct jaVector4i
{
	int x;
	int y;
	int z;
	int w;
};

struct jaVector2
{
	float x;
	float y;
};

JA_EXPORT struct jaVector2 jaVector2Clean();
JA_EXPORT struct jaVector2 jaVector2Set(float x, float y);

JA_EXPORT struct jaVector2 jaVector2Add(struct jaVector2 vec_a, struct jaVector2 vec_b);
JA_EXPORT struct jaVector2 jaVector2Subtract(struct jaVector2 vec_a, struct jaVector2 vec_b);
JA_EXPORT struct jaVector2 jaVector2Multiply(struct jaVector2 vec_a, struct jaVector2 vec_b);
JA_EXPORT struct jaVector2 jaVector2Divide(struct jaVector2 vec_a, struct jaVector2 vec_b);

JA_EXPORT struct jaVector2 jaVector2Invert(struct jaVector2 vec);
JA_EXPORT struct jaVector2 jaVector2Scale(struct jaVector2 vec, float scale);
JA_EXPORT struct jaVector2 jaVector2Normalize(struct jaVector2 vec);
JA_EXPORT float jaVector2Cross(struct jaVector2 vec_a, struct jaVector2 vec_b);

JA_EXPORT float jaVector2Length(struct jaVector2 vec);
JA_EXPORT float jaVector2Distance(struct jaVector2 vec_a, struct jaVector2 vec_b);
JA_EXPORT float jaVector2Dot(struct jaVector2 vec_a, struct jaVector2 vec_b);
JA_EXPORT bool jaVector2Equals(struct jaVector2 vec_a, struct jaVector2 vec_b);

struct jaVector3
{
	float x;
	float y;
	float z;
};

JA_EXPORT struct jaVector3 jaVector3Clean();
JA_EXPORT struct jaVector3 jaVector3Set(float x, float y, float z);

JA_EXPORT struct jaVector3 jaVector3Add(struct jaVector3 vec_a, struct jaVector3 vec_b);
JA_EXPORT struct jaVector3 jaVector3Subtract(struct jaVector3 vec_a, struct jaVector3 vec_b);
JA_EXPORT struct jaVector3 jaVector3Multiply(struct jaVector3 vec_a, struct jaVector3 vec_b);
JA_EXPORT struct jaVector3 jaVector3Divide(struct jaVector3 vec_a, struct jaVector3 vec_b);

JA_EXPORT struct jaVector3 jaVector3Invert(struct jaVector3 vec);
JA_EXPORT struct jaVector3 jaVector3Scale(struct jaVector3 vec, float scale);
JA_EXPORT struct jaVector3 jaVector3Normalize(struct jaVector3 vec);
JA_EXPORT struct jaVector3 jaVector3Cross(struct jaVector3 vec_a, struct jaVector3 vec_b);

JA_EXPORT float jaVector3Length(struct jaVector3 vec);
JA_EXPORT float jaVector3Distance(struct jaVector3 vec_a, struct jaVector3 vec_b);
JA_EXPORT float jaVector3Dot(struct jaVector3 vec_a, struct jaVector3 vec_b);
JA_EXPORT bool jaVector3Equals(struct jaVector3 vec_a, struct jaVector3 vec_b);

struct jaVector4
{
	float x;
	float y;
	float z;
	float w;
};

JA_EXPORT struct jaVector4 jaVector4Clean();
JA_EXPORT struct jaVector4 jaVector4Set(float x, float y, float z, float w);

JA_EXPORT struct jaVector4 jaVector4Add(struct jaVector4 vec_a, struct jaVector4 vec_b);
JA_EXPORT struct jaVector4 jaVector4Subtract(struct jaVector4 vec_a, struct jaVector4 vec_b);
JA_EXPORT struct jaVector4 jaVector4Multiply(struct jaVector4 vec_a, struct jaVector4 vec_b);
JA_EXPORT struct jaVector4 jaVector4Divide(struct jaVector4 vec_a, struct jaVector4 vec_b);

JA_EXPORT struct jaVector4 jaVector4Invert(struct jaVector4 vec);
JA_EXPORT struct jaVector4 jaVector4Scale(struct jaVector4 vec, float scale);
JA_EXPORT struct jaVector4 jaVector4Normalize(struct jaVector4 vec);

JA_EXPORT float jaVector4Length(struct jaVector4 vec);
JA_EXPORT float jaVector4Distance(struct jaVector4 vec_a, struct jaVector4 vec_b);
JA_EXPORT float jaVector4Dot(struct jaVector4 vec_a, struct jaVector4 vec_b);
JA_EXPORT bool jaVector4Equals(struct jaVector4 vec_a, struct jaVector4 vec_b);

#endif
