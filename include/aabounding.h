/*-----------------------------

 [aabounding.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
#endif

#ifndef AABOUNDING_H
#define AABOUNDING_H

	#include "vector.h"

	struct AabBox
	{
		struct Vector3 min;
		struct Vector3 max;
	};

	struct AabRectangle
	{
		struct Vector2 min;
		struct Vector2 max;
	};

	struct Sphere
	{
		struct Vector3 origin;
		float radius;
	};

	struct Circle
	{
		struct Vector2 origin;
		float radius;
	};

	DLL_EXP bool AabCollisionRectRect(struct AabRectangle a, struct AabRectangle b);
	DLL_EXP bool AabCollisionRectCircle(struct AabRectangle, struct Circle);

	DLL_EXP bool AabCollisionBoxBox(struct AabBox a, struct AabBox b);
	DLL_EXP bool AabCollisionBoxSphere(struct AabBox, struct Sphere);

	DLL_EXP struct AabRectangle AabToRectangle(struct AabBox);
	DLL_EXP struct AabBox AabToBox(struct AabRectangle, float min_z, float max_z);

	DLL_EXP struct Vector2 AabMiddleRect(struct AabRectangle);
	DLL_EXP struct Vector3 AabMiddleBox(struct AabBox);


	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define AabCollision(a, b) _Generic((a), \
		struct AabRectangle: _Generic((b), \
			struct AabRectangle: AabCollisionRectRect, \
			struct Circle: AabCollisionRectCircle, \
			default: AabCollisionRectRect \
		), \
		struct AabBox: _Generic((b), \
			struct AabBox: AabCollisionBoxBox, \
			struct Sphere: AabCollisionBoxSphere, \
			default: AabCollisionBoxBox \
		) \
	)(a, b)

	#define AabMiddle(obj) _Generic((obj), \
		struct AabRectangle: AabMiddleRect, \
		struct AabBox: AabMiddleBox, \
		default: AabMiddleRect \
	)(obj)

	#endif

#endif
