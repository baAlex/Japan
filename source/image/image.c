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
#include <stdlib.h>
#include <string.h>

#include "../common.h"
#include "japan-image.h"


extern bool CheckMagicSgi(uint16_t value);
extern struct jaImage* ImageLoadSgi(FILE* file, const char* filename, struct jaStatus* st);
extern int ImageExLoadSgi(FILE* file, struct jaImageEx* out, struct jaStatus* st);


/*-----------------------------

 jaImageCreate()
-----------------------------*/
struct jaImage* jaImageCreate(enum jaImageFormat format, size_t width, size_t height, size_t channels)
{
	struct jaImage* image = NULL;
	size_t size = (size_t)(jaBitsPerComponent(format) / 8) * width * height * channels;

	if ((image = malloc(sizeof(struct jaImage) + size)) != NULL)
	{
		image->width = width;
		image->height = height;
		image->channels = channels;
		image->format = format;
		image->size = size;
		image->data = ((struct jaImage*)image + 1);
	}

	return image;
}


/*-----------------------------

 jaImageDelete()
-----------------------------*/
inline void jaImageDelete(struct jaImage* image)
{
	free(image);
}


/*-----------------------------

 jaImageLoad()
-----------------------------*/
struct jaImage* jaImageLoad(const char* filename, struct jaStatus* st)
{
	FILE* file = NULL;
	struct jaImage* image = NULL;
	uint16_t magic = 0;

	jaStatusSet(st, "jaImageLoad", STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaImageLoad", STATUS_FS_ERROR, "'%s'", filename);
		return NULL;
	}

	if (fread(&magic, sizeof(uint16_t), 1, file) != 1)
	{
		jaStatusSet(st, "jaImageLoad", STATUS_UNEXPECTED_EOF, "near magic ('%s')", filename);
		goto return_failure;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicSgi(magic) == true)
		image = ImageLoadSgi(file, filename, st);
	// else if (CheckMagicBmp(magic) == true)
	//	image = ImageLoadBmp(file, filename, st;
	else
	{
		jaStatusSet(st, "jaImageLoad", STATUS_UNKNOWN_FILE_FORMAT, "'%s'", filename);
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

 jaImageSaveRaw()
-----------------------------*/
int jaImageSaveRaw(const struct jaImage* image, const char* filename, struct jaStatus* st)
{
	FILE* file = NULL;

	jaStatusSet(st, "jaImageSaveRaw", STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "wb")) == NULL)
	{
		jaStatusSet(st, "jaImageSaveRaw", STATUS_FS_ERROR, "'%s'", filename);
		return 1;
	}

	if (fwrite(image->data, image->size, 1, file) != 1)
	{
		jaStatusSet(st, "jaImageSaveRaw", STATUS_IO_ERROR, "'%s'", filename);
		fclose(file);
		return 1;
	}

	fclose(file);
	return 0;
}


/*-----------------------------

 jaImageExLoad()
-----------------------------*/
int jaImageExLoad(FILE* file, struct jaImageEx* out, struct jaStatus* st)
{
	uint16_t magic = 0;

	jaStatusSet(st, "jaImageExLoad", STATUS_SUCCESS, NULL);

	if (fread(&magic, sizeof(uint16_t), 1, file) != 1)
	{
		jaStatusSet(st, "jaImageExLoad", STATUS_UNEXPECTED_EOF, "near magic");
		return 1;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicSgi(magic) == true)
		return ImageExLoadSgi(file, out, st);
	// else if (CheckMagicBmp(magic) == true)
	//	return ImageExLoadBmp(file, out, st);

	// Unsuccessfully bye!
	jaStatusSet(st, "jaImageExLoad", STATUS_UNKNOWN_FILE_FORMAT, NULL);
	return 1;
}


/*-----------------------------

 jaBytesPerPixel()
-----------------------------*/
inline int jaBytesPerPixel(const struct jaImage* image)
{
	switch (image->format)
	{
	case IMAGE_U8: return (1 * (int)image->channels);
	case IMAGE_U16: return (2 * (int)image->channels);
	case IMAGE_FLOAT: return (4 * (int)image->channels);
	}

	return 0;
}


/*-----------------------------

 jaBitsPerComponent()
-----------------------------*/
inline int jaBitsPerComponent(enum jaImageFormat format)
{
	switch (format)
	{
	case IMAGE_U8: return 8;
	case IMAGE_U16: return 16;
	case IMAGE_FLOAT: return 32;
	}

	return 0;
}
