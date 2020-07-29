/*-----------------------------

 [images.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

#include "japan-image.h"

#include <string.h>


const char* sBasename(const char* filename)
{
	const char* s = NULL;

	// Inefficient
	if ((s = strrchr(filename, '/')) == NULL)
		s = strrchr(filename, '\\');

	return s;
}

// Borrowed from dictionary.c
static uint64_t sFNV1Hash(const char* key, size_t size)
{
#define FNV_OFFSET_BASIS 0xCBF29CE484222325
#define FNV_PRIME 0x100000001B3

	uint64_t hash = FNV_OFFSET_BASIS;

	for (size_t i = 0; i < size; i++)
	{
		hash = hash ^ (uint64_t)key[i];
		hash = hash * FNV_PRIME;
	}

	return hash;
}


static uint64_t Validate(const char* filename, size_t expected_width, size_t expected_height, size_t expected_channels,
                         enum jaImageFormat expected_format)
{
	struct jaStatus st = {0};
	struct jaImage* image = NULL;

	char temp_filename[128];

	if ((image = jaImageLoad(filename, &st)) == NULL)
	{
		jaStatusPrint("ImageTest1_Sgi", st);
		assert_false(true);
	}

	assert_true((image->width == expected_width));
	assert_true((image->height == expected_height));
	assert_true((image->channels == expected_channels));

	assert_true((image->size == (size_t)(jaBitsPerComponent(expected_format) / 8) * expected_width * expected_height *
	                                expected_channels));

	uint64_t hash = sFNV1Hash(image->data, image->size);

	{
		sprintf(temp_filename, "./tests/out/%s.data", sBasename(filename));

		if (jaImageSaveRaw(image, temp_filename, &st) != 0)
		{
			jaStatusPrint("ImageTest1_Sgi", st);
			assert_false(true);
		}
	}

	jaImageDelete(image);

	return hash;
}

void ImageTest1_Sgi(void** cmocka_state)
{
	(void)cmocka_state;

	uint64_t hash = Validate("./tests/akabeko-gray8.sgi", 360, 240, 1, JA_IMAGE_U8);
	assert_true((Validate("./tests/akabeko-gray8-rle.sgi", 360, 240, 1, JA_IMAGE_U8) == hash));

	hash = Validate("./tests/akabeko-graya8.sgi", 360, 240, 2, JA_IMAGE_U8);
	assert_true((Validate("./tests/akabeko-graya8-rle.sgi", 360, 240, 2, JA_IMAGE_U8) == hash));

	hash = Validate("./tests/akabeko-rgb8.sgi", 360, 240, 3, JA_IMAGE_U8);
	assert_true((Validate("./tests/akabeko-rgb8-rle.sgi", 360, 240, 3, JA_IMAGE_U8) == hash));

	hash = Validate("./tests/akabeko-rgba8.sgi", 360, 240, 4, JA_IMAGE_U8);
	assert_true((Validate("./tests/akabeko-rgba8-rle.sgi", 360, 240, 4, JA_IMAGE_U8) == hash));
}
