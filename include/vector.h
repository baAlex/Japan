/*-----------------------------

 [vector.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef VECTOR_H
#define VECTOR_H

	#include <stdbool.h>

	struct Vector
	{
		float x;
		float y;
		float z;
	};

	struct VectorInt
	{
		int x;
		int y;
		int z;
	};

	struct Vector VectorClean();
	struct Vector VectorSet(float x, float y, float z);

	struct Vector VectorAdd(struct Vector vec_a, struct Vector vec_b);
	struct Vector VectorSubtract(struct Vector vec_a, struct Vector vec_b);
	struct Vector VectorMultiply(struct Vector vec_a, struct Vector vec_b);
	struct Vector VectorDivide(struct Vector vec_a, struct Vector vec_b);

	struct Vector VectorInvert(struct Vector vec);
	struct Vector VectorScale(struct Vector vec, float scale);
	struct Vector VectorNormalize(struct Vector vec);

	float VectorLength(struct Vector vec);
	float VectorDistance(struct Vector vec_a, struct Vector vec_b);
	bool VectorEquals(struct Vector vec_a, struct Vector vec_b);

#endif
