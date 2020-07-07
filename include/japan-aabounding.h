/*-----------------------------

 [japan-aabounding.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_AABOUNDING_H
#define JAPAN_AABOUNDING_H

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

struct jaAABBox
{
	struct jaVector3 min;
	struct jaVector3 max;
};

struct jaAABRectangle
{
	struct jaVector2 min;
	struct jaVector2 max;
};

struct jaSphere
{
	struct jaVector3 origin;
	float radius;
};

struct jaCircle
{
	struct jaVector2 origin;
	float radius;
};

JA_EXPORT bool jaAABCollisionRectRect(struct jaAABRectangle a, struct jaAABRectangle b);
JA_EXPORT bool jaAABCollisionRectCircle(struct jaAABRectangle, struct jaCircle);

JA_EXPORT bool jaAABCollisionBoxBox(struct jaAABBox a, struct jaAABBox b);
JA_EXPORT bool jaAABCollisionBoxSphere(struct jaAABBox, struct jaSphere);

JA_EXPORT struct jaAABRectangle jaAABToRectangle(struct jaAABBox);
JA_EXPORT struct jaAABBox jaAABToBox(struct jaAABRectangle, float min_z, float max_z);

JA_EXPORT struct jaVector2 jaAABMiddleRect(struct jaAABRectangle);
JA_EXPORT struct jaVector3 jaAABMiddleBox(struct jaAABBox);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define jaAABCollision(a, b)\
	_Generic((a),\
		struct jaAABRectangle : _Generic((b),\
			struct jaAABRectangle : jaAABCollisionRectRect,\
			struct jaCircle : jaAABCollisionRectCircle,\
			default : jaAABCollisionRectRect),\
		struct jaAABBox : _Generic((b),\
			struct jaAABBox : jaAABCollisionBoxBox,\
			struct jaSphere : jaAABCollisionBoxSphere,\
			default : jaAABCollisionBoxBox)\
	)(a, b)

#define jaAABMiddle(obj)\
	_Generic((obj),\
		struct jaAABRectangle : jaAABMiddleRect,\
		struct jaAABBox : jaAABMiddleBox,\
		default : jaAABMiddleRect\
	)(obj)

#endif

#endif
