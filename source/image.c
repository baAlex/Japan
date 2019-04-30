/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [image.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"

extern bool CheckMagicSgi(uint16_t value);
// extern bool CheckMagicBmp(uint16_t value);

extern struct Image* ImageLoadSgi(FILE* file, const char* filename, struct Error* e);
// extern struct Image* ImageLoadBmp(FILE* file, const char* filename, struct Error* e);


/*-----------------------------

 ImageCreate()
-----------------------------*/
struct Image* ImageCreate(enum ImageFormat format, size_t width, size_t height)
{
	struct Image* image = NULL;
	size_t size = 0;

	switch (format)
	{
	case IMAGE_GRAY8:
		size = sizeof(uint8_t) * width * height;
		break;
	case IMAGE_GRAYA8:
		size = sizeof(uint8_t) * width * height * 2; // 2 channels
		break;
	case IMAGE_RGB8:
		size = sizeof(uint8_t) * width * height * 3; // 3 channels
		break;
	case IMAGE_RGBA8:
		size = sizeof(uint8_t) * width * height * 4; // ...
		break;
	case IMAGE_GRAY16:
		size = sizeof(uint16_t) * width * height;
		break;
	case IMAGE_GRAYA16:
		size = sizeof(uint8_t) * width * height * 2;
		break;
	case IMAGE_RGB16:
		size = sizeof(uint8_t) * width * height * 3;
		break;
	case IMAGE_RGBA16:
		size = sizeof(uint8_t) * width * height * 4;
		break;
	}

	if ((image = malloc(sizeof(struct Image) + size)) != NULL)
	{
		image->width = width;
		image->height = height;
		image->size = size;
		image->format = format;
		image->data = ((struct Image*)image + 1);
	}

	return image;
}


/*-----------------------------

 ImageDelete()
-----------------------------*/
void ImageDelete(struct Image* image) { free(image); }


/*-----------------------------

 ImageLoad()
-----------------------------*/
struct Image* ImageLoad(const char* filename, struct Error* e)
{
	FILE* file = NULL;
	struct Image* image = NULL;
	uint16_t magic = 0;

	ErrorSet(e, NO_ERROR, NULL, NULL);

	if ((file = fopen(filename, "rb")) == NULL)
	{
		ErrorSet(e, ERROR_FS, "ImageLoad", "'%s'", filename);
		return NULL;
	}

	if (fread(&magic, sizeof(uint16_t), 1, file) != 1)
	{
		ErrorSet(e, ERROR_BROKEN, "ImageLoad", "magic ('%s')", filename);
		goto return_failure;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicSgi(magic) == true)
		image = ImageLoadSgi(file, filename, e);
	// else if (CheckMagicBmp(magic) == true)
	//	image = ImageLoadBmp(file, filename, e);
	else
	{
		ErrorSet(e, ERROR_UNKNOWN_FORMAT, "ImageLoad", "'%s'", filename);
		goto return_failure;
	}

	// Bye!
	fclose(file);
	return image;

return_failure:
	fclose(file);
	return NULL;
}


/*-----------------------------

 ImageSaveRaw()
-----------------------------*/
struct Error ImageSaveRaw(struct Image* image, const char* filename)
{
	struct Error e = {.code = NO_ERROR};
	FILE* file = NULL;

	if ((file = fopen(filename, "wb")) == NULL)
	{
		ErrorSet(&e, ERROR_FS, "ImageSaveRaw", "'%s'", filename);
		return e;
	}

	if (fwrite(image->data, image->size, 1, file) != 1)
	{
		ErrorSet(&e, ERROR_IO, "ImageSaveRaw", "'%s'", filename);
		fclose(file);
		return e;
	}

	fclose(file);
	return e;
}
