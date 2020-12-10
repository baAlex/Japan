/*-----------------------------

 [japan-vector.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_VECTOR_H
#define JAPAN_VECTOR_H

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

struct jaVector2
{
	float x;
	float y;
};

JA_EXPORT struct jaVector2 jaVector2Add(struct jaVector2 va, struct jaVector2 vb);
JA_EXPORT struct jaVector2 jaVector2Subtract(struct jaVector2 va, struct jaVector2 vb);
JA_EXPORT struct jaVector2 jaVector2Multiply(struct jaVector2 va, struct jaVector2 vb);
JA_EXPORT struct jaVector2 jaVector2Divide(struct jaVector2 va, struct jaVector2 vb);

JA_EXPORT struct jaVector2 jaVector2Invert(struct jaVector2 v);
JA_EXPORT struct jaVector2 jaVector2Scale(struct jaVector2 v, float scale);
JA_EXPORT struct jaVector2 jaVector2Normalize(struct jaVector2 v);
JA_EXPORT float jaVector2Cross(struct jaVector2 va, struct jaVector2 vb);

JA_EXPORT float jaVector2Length(struct jaVector2 v);
JA_EXPORT float jaVector2Distance(struct jaVector2 va, struct jaVector2 vb);
JA_EXPORT float jaVector2Dot(struct jaVector2 va, struct jaVector2 vb);
JA_EXPORT bool jaVector2Equals(struct jaVector2 va, struct jaVector2 vb);

struct jaVector3
{
	float x;
	float y;
	float z;
};

JA_EXPORT struct jaVector3 jaVector3Add(struct jaVector3 va, struct jaVector3 vb);
JA_EXPORT struct jaVector3 jaVector3Subtract(struct jaVector3 va, struct jaVector3 vb);
JA_EXPORT struct jaVector3 jaVector3Multiply(struct jaVector3 va, struct jaVector3 vb);
JA_EXPORT struct jaVector3 jaVector3Divide(struct jaVector3 va, struct jaVector3 vb);

JA_EXPORT struct jaVector3 jaVector3Invert(struct jaVector3 v);
JA_EXPORT struct jaVector3 jaVector3Scale(struct jaVector3 v, float scale);
JA_EXPORT struct jaVector3 jaVector3Normalize(struct jaVector3 v);
JA_EXPORT struct jaVector3 jaVector3Cross(struct jaVector3 va, struct jaVector3 vb);

JA_EXPORT float jaVector3Length(struct jaVector3 v);
JA_EXPORT float jaVector3Distance(struct jaVector3 va, struct jaVector3 vb);
JA_EXPORT float jaVector3Dot(struct jaVector3 va, struct jaVector3 vb);
JA_EXPORT bool jaVector3Equals(struct jaVector3 va, struct jaVector3 vb);

struct jaVector4
{
	float x;
	float y;
	float z;
	float w;
};

JA_EXPORT struct jaVector4 jaVector4Add(struct jaVector4 va, struct jaVector4 vb);
JA_EXPORT struct jaVector4 jaVector4Subtract(struct jaVector4 va, struct jaVector4 vb);
JA_EXPORT struct jaVector4 jaVector4Multiply(struct jaVector4 va, struct jaVector4 vb);
JA_EXPORT struct jaVector4 jaVector4Divide(struct jaVector4 va, struct jaVector4 vb);

JA_EXPORT struct jaVector4 jaVector4Invert(struct jaVector4 v);
JA_EXPORT struct jaVector4 jaVector4Scale(struct jaVector4 v, float scale);
JA_EXPORT struct jaVector4 jaVector4Normalize(struct jaVector4 v);

JA_EXPORT float jaVector4Length(struct jaVector4 v);
JA_EXPORT float jaVector4Distance(struct jaVector4 va, struct jaVector4 vb);
JA_EXPORT float jaVector4Dot(struct jaVector4 va, struct jaVector4 vb);
JA_EXPORT bool jaVector4Equals(struct jaVector4 va, struct jaVector4 vb);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaVectorAdd(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Add,\
		struct jaVector3 : jaVector3Add,\
		struct jaVector4 : jaVector4Add,\
		default : jaVector2Add\
	)(va, vb)

#define jaVectorSubtract(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Subtract,\
		struct jaVector3 : jaVector3Subtract,\
		struct jaVector4 : jaVector4Subtract,\
		default : jaVector2Subtract\
	)(va, vb)

#define jaVectorMultiply(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Multiply,\
		struct jaVector3 : jaVector3Multiply,\
		struct jaVector4 : jaVector4Multiply,\
		default : jaVector2Multiply\
	)(va, vb)

#define jaVectorDivide(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Divide,\
		struct jaVector3 : jaVector3Divide,\
		struct jaVector4 : jaVector4Divide,\
		default : jaVector2Divide\
	)(va, vb)

#define jaVectorInvert(v)\
	_Generic((v),\
		struct jaVector2 : jaVector2Invert,\
		struct jaVector3 : jaVector3Invert,\
		struct jaVector4 : jaVector4Invert,\
		default : jaVector2Invert\
	)(v)

#define jaVectorScale(v, s)\
	_Generic((v),\
		struct jaVector2 : jaVector2Scale,\
		struct jaVector3 : jaVector3Scale,\
		struct jaVector4 : jaVector4Scale,\
		default : jaVector2Scale\
	)(v, s)

#define jaVectorNormalize(v)\
	_Generic((v),\
		struct jaVector2 : jaVector2Normalize,\
		struct jaVector3 : jaVector3Normalize,\
		struct jaVector4 : jaVector4Normalize,\
		default : jaVector2Normalize\
	)(v)

#define jaVectorCross(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Cross,\
		struct jaVector3 : jaVector3Cross,\
		default : jaVector2Cross\
	)(va, vb)

#define jaVectorLength(v)\
	_Generic((v),\
		struct jaVector2 : jaVector2Length,\
		struct jaVector3 : jaVector3Length,\
		struct jaVector4 : jaVector4Length,\
		default : jaVector2Length\
	)(v)

#define jaVectorDistance(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Distance,\
		struct jaVector3 : jaVector3Distance,\
		struct jaVector4 : jaVector4Distance,\
		default : jaVector2Distance\
	)(va, vb)

#define jaVectorDot(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Dot,\
		struct jaVector3 : jaVector3Dot,\
		struct jaVector4 : jaVector4Dot,\
		default : jaVector2Dot\
	)(va, vb)

#define jaVectorEquals(va, vb)\
	_Generic((va),\
		struct jaVector2 : jaVector2Equals,\
		struct jaVector3 : jaVector3Equals,\
		struct jaVector4 : jaVector4Equals,\
		default : jaVector2Equals\
	)(va, vb)

#endif

#endif
