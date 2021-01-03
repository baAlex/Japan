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

struct jaVectorF2
{
	float x;
	float y;
};

JA_EXPORT struct jaVectorF2 jaVectorAddF2(struct jaVectorF2 va, struct jaVectorF2 vb);
JA_EXPORT struct jaVectorF2 jaVectorSubtractF2(struct jaVectorF2 va, struct jaVectorF2 vb);
JA_EXPORT struct jaVectorF2 jaVectorMultiplyF2(struct jaVectorF2 va, struct jaVectorF2 vb);
JA_EXPORT struct jaVectorF2 jaVectorDivideF2(struct jaVectorF2 va, struct jaVectorF2 vb);

JA_EXPORT struct jaVectorF2 jaVectorInvertF2(struct jaVectorF2 v);
JA_EXPORT struct jaVectorF2 jaVectorScaleF2(struct jaVectorF2 v, float scale);
JA_EXPORT struct jaVectorF2 jaVectorNormalizeF2(struct jaVectorF2 v);
JA_EXPORT float jaVectorCrossF2(struct jaVectorF2 va, struct jaVectorF2 vb);

JA_EXPORT float jaVectorLengthF2(struct jaVectorF2 v);
JA_EXPORT float jaVectorDistanceF2(struct jaVectorF2 va, struct jaVectorF2 vb);
JA_EXPORT float jaVectorDotF2(struct jaVectorF2 va, struct jaVectorF2 vb);
JA_EXPORT bool jaVectorEqualsF2(struct jaVectorF2 va, struct jaVectorF2 vb);

struct jaVectorF3
{
	float x;
	float y;
	float z;
};

JA_EXPORT struct jaVectorF3 jaVectorAddF3(struct jaVectorF3 va, struct jaVectorF3 vb);
JA_EXPORT struct jaVectorF3 jaVectorSubtractF3(struct jaVectorF3 va, struct jaVectorF3 vb);
JA_EXPORT struct jaVectorF3 jaVectorMultiplyF3(struct jaVectorF3 va, struct jaVectorF3 vb);
JA_EXPORT struct jaVectorF3 jaVectorDivideF3(struct jaVectorF3 va, struct jaVectorF3 vb);

JA_EXPORT struct jaVectorF3 jaVectorInvertF3(struct jaVectorF3 v);
JA_EXPORT struct jaVectorF3 jaVectorScaleF3(struct jaVectorF3 v, float scale);
JA_EXPORT struct jaVectorF3 jaVectorNormalizeF3(struct jaVectorF3 v);
JA_EXPORT struct jaVectorF3 jaVectorCrossF3(struct jaVectorF3 va, struct jaVectorF3 vb);

JA_EXPORT float jaVectorLengthF3(struct jaVectorF3 v);
JA_EXPORT float jaVectorDistanceF3(struct jaVectorF3 va, struct jaVectorF3 vb);
JA_EXPORT float jaVectorDotF3(struct jaVectorF3 va, struct jaVectorF3 vb);
JA_EXPORT bool jaVectorEqualsF3(struct jaVectorF3 va, struct jaVectorF3 vb);

struct jaVectorF4
{
	float x;
	float y;
	float z;
	float w;
};

JA_EXPORT struct jaVectorF4 jaVectorAddF4(struct jaVectorF4 va, struct jaVectorF4 vb);
JA_EXPORT struct jaVectorF4 jaVectorSubtractF4(struct jaVectorF4 va, struct jaVectorF4 vb);
JA_EXPORT struct jaVectorF4 jaVectorMultiplyF4(struct jaVectorF4 va, struct jaVectorF4 vb);
JA_EXPORT struct jaVectorF4 jaVectorDivideF4(struct jaVectorF4 va, struct jaVectorF4 vb);

JA_EXPORT struct jaVectorF4 jaVectorInvertF4(struct jaVectorF4 v);
JA_EXPORT struct jaVectorF4 jaVectorScaleF4(struct jaVectorF4 v, float scale);
JA_EXPORT struct jaVectorF4 jaVectorNormalizeF4(struct jaVectorF4 v);

JA_EXPORT float jaVectorLengthF4(struct jaVectorF4 v);
JA_EXPORT float jaVectorDistanceF4(struct jaVectorF4 va, struct jaVectorF4 vb);
JA_EXPORT float jaVectorDotF4(struct jaVectorF4 va, struct jaVectorF4 vb);
JA_EXPORT bool jaVectorEqualsF4(struct jaVectorF4 va, struct jaVectorF4 vb);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaVectorAdd(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorAddF2,\
		struct jaVectorF3 : jaVectorAddF3,\
		struct jaVectorF4 : jaVectorAddF4,\
		default : jaVectorAddF2\
	)(va, vb)

#define jaVectorSubtract(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorSubtractF2,\
		struct jaVectorF3 : jaVectorSubtractF3,\
		struct jaVectorF4 : jaVectorSubtractF4,\
		default : jaVectorSubtractF2\
	)(va, vb)

#define jaVectorMultiply(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorMultiplyF2,\
		struct jaVectorF3 : jaVectorMultiplyF3,\
		struct jaVectorF4 : jaVectorMultiplyF4,\
		default : jaVectorMultiplyF2\
	)(va, vb)

#define jaVectorDivide(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorDivideF2,\
		struct jaVectorF3 : jaVectorDivideF3,\
		struct jaVectorF4 : jaVectorDivideF4,\
		default : jaVectorDivideF2\
	)(va, vb)

#define jaVectorInvert(v)\
	_Generic((v),\
		struct jaVectorF2 : jaVectorInvertF2,\
		struct jaVectorF3 : jaVectorInvertF3,\
		struct jaVectorF4 : jaVectorInvertF4,\
		default : jaVectorInvertF2\
	)(v)

#define jaVectorScale(v, s)\
	_Generic((v),\
		struct jaVectorF2 : jaVectorScaleF2,\
		struct jaVectorF3 : jaVectorScaleF3,\
		struct jaVectorF4 : jaVectorScaleF4,\
		default : jaVectorScaleF2\
	)(v, s)

#define jaVectorNormalize(v)\
	_Generic((v),\
		struct jaVectorF2 : jaVectorNormalizeF2,\
		struct jaVectorF3 : jaVectorNormalizeF3,\
		struct jaVectorF4 : jaVectorNormalizeF4,\
		default : jaVectorNormalizeF2\
	)(v)

#define jaVectorCross(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorCrossF2,\
		struct jaVectorF3 : jaVectorCrossF3,\
		default : jaVectorCrossF2\
	)(va, vb)

#define jaVectorLength(v)\
	_Generic((v),\
		struct jaVectorF2 : jaVectorLengthF2,\
		struct jaVectorF3 : jaVectorLengthF3,\
		struct jaVectorF4 : jaVectorLengthF4,\
		default : jaVectorLengthF2\
	)(v)

#define jaVectorDistance(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorDistanceF2,\
		struct jaVectorF3 : jaVectorDistanceF3,\
		struct jaVectorF4 : jaVectorDistanceF4,\
		default : jaVectorDistanceF2\
	)(va, vb)

#define jaVectorDot(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorDotF2,\
		struct jaVectorF3 : jaVectorDotF3,\
		struct jaVectorF4 : jaVectorDotF4,\
		default : jaVectorDotF2\
	)(va, vb)

#define jaVectorEquals(va, vb)\
	_Generic((va),\
		struct jaVectorF2 : jaVectorEqualsF2,\
		struct jaVectorF3 : jaVectorEqualsF3,\
		struct jaVectorF4 : jaVectorEqualsF4,\
		default : jaVectorEqualsF2\
	)(va, vb)

#endif

#endif
