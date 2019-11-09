/*-----------------------------

 [vector.h]
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

#ifndef VECTOR_H
#define VECTOR_H

	#include <stdbool.h>

	struct Vector2i { int x, y; };
	struct Vector3i { int x, y, z; };
	struct Vector4i { int x, y, u; };

	struct Vector2
	{
		float x;
		float y;
	};

	DLL_EXP struct Vector2 Vector2Clean();
	DLL_EXP struct Vector2 Vector2Set(float x, float y);

	DLL_EXP struct Vector2 Vector2Add(struct Vector2 vec_a, struct Vector2 vec_b);
	DLL_EXP struct Vector2 Vector2Subtract(struct Vector2 vec_a, struct Vector2 vec_b);
	DLL_EXP struct Vector2 Vector2Multiply(struct Vector2 vec_a, struct Vector2 vec_b);
	DLL_EXP struct Vector2 Vector2Divide(struct Vector2 vec_a, struct Vector2 vec_b);

	DLL_EXP struct Vector2 Vector2Invert(struct Vector2 vec);
	DLL_EXP struct Vector2 Vector2Scale(struct Vector2 vec, float scale);
	DLL_EXP struct Vector2 Vector2Normalize(struct Vector2 vec);
	DLL_EXP float Vector2Cross(struct Vector2 vec_a, struct Vector2 vec_b);

	DLL_EXP float Vector2Length(struct Vector2 vec);
	DLL_EXP float Vector2Distance(struct Vector2 vec_a, struct Vector2 vec_b);
	DLL_EXP float Vector2Dot(struct Vector2 vec_a, struct Vector2 vec_b);
	DLL_EXP bool Vector2Equals(struct Vector2 vec_a, struct Vector2 vec_b);

	struct Vector3
	{
		float x;
		float y;
		float z;
	};

	DLL_EXP struct Vector3 Vector3Clean();
	DLL_EXP struct Vector3 Vector3Set(float x, float y, float z);

	DLL_EXP struct Vector3 Vector3Add(struct Vector3 vec_a, struct Vector3 vec_b);
	DLL_EXP struct Vector3 Vector3Subtract(struct Vector3 vec_a, struct Vector3 vec_b);
	DLL_EXP struct Vector3 Vector3Multiply(struct Vector3 vec_a, struct Vector3 vec_b);
	DLL_EXP struct Vector3 Vector3Divide(struct Vector3 vec_a, struct Vector3 vec_b);

	DLL_EXP struct Vector3 Vector3Invert(struct Vector3 vec);
	DLL_EXP struct Vector3 Vector3Scale(struct Vector3 vec, float scale);
	DLL_EXP struct Vector3 Vector3Normalize(struct Vector3 vec);
	DLL_EXP struct Vector3 Vector3Cross(struct Vector3 vec_a, struct Vector3 vec_b);

	DLL_EXP float Vector3Length(struct Vector3 vec);
	DLL_EXP float Vector3Distance(struct Vector3 vec_a, struct Vector3 vec_b);
	DLL_EXP float Vector3Dot(struct Vector3 vec_a, struct Vector3 vec_b);
	DLL_EXP bool Vector3Equals(struct Vector3 vec_a, struct Vector3 vec_b);

	struct Vector4
	{
		float x;
		float y;
		float z;
		float u;
	};

	DLL_EXP struct Vector4 Vector4Clean();
	DLL_EXP struct Vector4 Vector4Set(float x, float y, float z, float u);

	DLL_EXP struct Vector4 Vector4Add(struct Vector4 vec_a, struct Vector4 vec_b);
	DLL_EXP struct Vector4 Vector4Subtract(struct Vector4 vec_a, struct Vector4 vec_b);
	DLL_EXP struct Vector4 Vector4Multiply(struct Vector4 vec_a, struct Vector4 vec_b);
	DLL_EXP struct Vector4 Vector4Divide(struct Vector4 vec_a, struct Vector4 vec_b);

	DLL_EXP struct Vector4 Vector4Invert(struct Vector4 vec);
	DLL_EXP struct Vector4 Vector4Scale(struct Vector4 vec, float scale);
	DLL_EXP struct Vector4 Vector4Normalize(struct Vector4 vec);

	DLL_EXP float Vector4Length(struct Vector4 vec);
	DLL_EXP float Vector4Distance(struct Vector4 vec_a, struct Vector4 vec_b);
	DLL_EXP float Vector4Dot(struct Vector4 vec_a, struct Vector4 vec_b);
	DLL_EXP bool Vector4Equals(struct Vector4 vec_a, struct Vector4 vec_b);

#endif
