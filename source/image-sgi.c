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

 [image-sgi.c]
 - Alexander Brandt 2019

 http://paulbourke.net/dataformats/sgirgb/sgiversion.html
 https://www.fileformat.info/format/sgiimage/egff.htm
-----------------------------*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "endianness.h"
#include "image.h"

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


extern int Channels(enum ImageFormat format);
int ImageExLoadSgi(FILE* file, struct ImageEx* out, struct Status* st);


#define SGI_MAGIC 474

struct SgiHead
{
	int16_t magic;

	int8_t compression;
	int8_t precision; // In bytes per pixel component
	uint16_t dimension;

	uint16_t x_size;
	uint16_t y_size;
	uint16_t z_size; // Or 'channels'

	int32_t min;
	int32_t max;

	char dummy[4];
	char name[80];

	int32_t pixel_type;
};


/*-----------------------------

 sPlotPixel_8()
-----------------------------*/
static inline void sPlotPixel_8(int channel, int row, int col, struct Image* image, uint8_t value)
{
	if (image->format == IMAGE_GRAY8 && channel < 1)
	{
		struct
		{
			uint8_t c[1];
		}* data = image->data;

		data += col + (image->width * row);
		data->c[channel] = value;
	}
	else if (image->format == IMAGE_GRAYA8 && channel < 2)
	{
		struct
		{
			uint8_t c[2];
		}* data = image->data;

		data += col + (image->width * row);
		data->c[channel] = value;
	}
	else if (image->format == IMAGE_RGB8 && channel < 3)
	{
		struct
		{
			uint8_t c[3];
		}* data = image->data;

		data += col + (image->width * row);
		data->c[channel] = value;
	}
	else if (image->format == IMAGE_RGBA8 && channel < 4)
	{
		struct
		{
			uint8_t c[4];
		}* data = image->data;

		data += col + (image->width * row);
		data->c[channel] = value;
	}
}


/*-----------------------------

 sReadUncompressed_8()
-----------------------------*/
static inline int sReadUncompressed_8(FILE* file, struct Image* image)
{
	uint8_t pixel = 0;

	for (int channel = 0; channel < Channels(image->format); channel++)
	{
		for (int row = ((int)image->height - 1); row >= 0; row--)
		{
			for (int col = 0; col < (int)image->width; col++)
			{
				if (fread(&pixel, 1, 1, file) != 1)
					return 1;

				sPlotPixel_8(channel, row, col, image, pixel);
			}
		}
	}

	return 0;
}


/*-----------------------------

 sReadCompressed_8()
-----------------------------*/
static int sReadCompressed_8(FILE* file, struct Image* image)
{
	void* buffer = NULL;
	size_t table_len = image->height * Channels(image->format);
	uint32_t* offset_table = NULL;
	uint32_t* size_table = NULL;

	// Offset and size tables of RLE scanlines
	if ((buffer = malloc(sizeof(uint32_t) * table_len * 2)) == NULL)
		return 1;

	offset_table = (uint32_t*)buffer;
	size_table = (uint32_t*)buffer + table_len;

	if (fread(offset_table, sizeof(uint32_t), table_len, file) != table_len)
		goto return_failure;

	if (fread(size_table, sizeof(uint32_t), table_len, file) != table_len)
		goto return_failure;

	for (size_t i = 0; i < table_len; i++)
	{
		offset_table[i] = EndianTo_32(offset_table[i], ENDIAN_BIG, ENDIAN_SYSTEM);
		size_table[i] = EndianTo_32(size_table[i], ENDIAN_BIG, ENDIAN_SYSTEM);
	}

	// Data
	for (int channel = 0; channel < Channels(image->format); channel++)
	{
		for (int row = 0; row < (int)image->height; row++)
		{
			int col = 0;

			if (fseek(file, offset_table[channel * image->height + (image->height - row - 1)], SEEK_SET) != 0)
				goto return_failure;

			// RLE scanline
			for (uint32_t i = 0; i < size_table[channel * image->height + (image->height - row - 1)]; i++)
			{
				uint8_t instruction = 0;
				uint8_t value = 0;
				uint8_t steps = 0;

				if (fread(&instruction, 1, 1, file) != 1)
					goto return_failure;

				steps = (instruction & 0x7F); // 0b01111111

				// Repeat next byte value x steps
				if ((instruction >> 7) == 0)
				{
					if (instruction == 0) // Repeat 0 steps = stop
						break;

					if (fread(&value, 1, 1, file) != 1)
						goto return_failure;

					for (uint8_t s = 0; s < steps; s++)
						sPlotPixel_8(channel, row, col + s, image, value);
				}

				// Read following bytes values for x steps
				else
				{
					for (uint8_t s = 0; s < steps; s++)
					{
						if (fread(&value, 1, 1, file) != 1)
							goto return_failure;

						sPlotPixel_8(channel, row, col + s, image, value);
					}
				}

				col += steps;
			}

			// Failure if scanline is broken
			if (col != (int)image->width)
				goto return_failure;
		}
	}

	// Bye!
	free(buffer);
	return 0;

return_failure:
	free(buffer);
	return 1;
}


/*-----------------------------

 CheckMagicSgi()
-----------------------------*/
bool CheckMagicSgi(uint16_t value)
{
	if (EndianTo_16(value, ENDIAN_BIG, ENDIAN_SYSTEM) == SGI_MAGIC)
		return true;

	return false;
}


/*-----------------------------

 ImageLoadSgi()
-----------------------------*/
struct Image* ImageLoadSgi(FILE* file, const char* filename, struct Status* st)
{
	struct ImageEx ex = {0};
	struct Image* image = NULL;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	if (ImageExLoadSgi(file, &ex, st) != 0)
		goto return_failure;

	DEBUG_PRINT("(Sgi) '%s':\n", filename);
	DEBUG_PRINT(" - Dimensions: %zux%zu px\n", ex.width, ex.height);
	DEBUG_PRINT(" - Uncompressed size: %zu bytes\n", ex.uncompressed_size);
	DEBUG_PRINT(" - Endianness: %s\n", (ex.endianness == ENDIAN_LITTLE) ? "little" : "big");
	DEBUG_PRINT(" - Compression: %s (%u)\n", (ex.compression == IMAGE_SGI_RLE) ? "rle" : "none", ex.compression);
	DEBUG_PRINT(" - Format: %i\n", ex.format);
	DEBUG_PRINT(" - Data offset: 0x%zX\n", ex.data_offset);

	// Data
	if (fseek(file, ex.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "ImageLoadSgi", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	if ((image = ImageCreate(ex.format, ex.width, ex.height)) == NULL)
		goto return_failure;

	if (ex.compression == IMAGE_SGI_RLE)
	{
		if (sReadCompressed_8(file, image) != 0)
		{
			StatusSet(st, "ImageLoadSgi", STATUS_UNEXPECTED_EOF, "reading compressed data ('%s')", filename);
			goto return_failure;
		}
	}
	else
	{
		if (sReadUncompressed_8(file, image) != 0)
		{
			StatusSet(st, "ImageLoadSgi", STATUS_UNEXPECTED_EOF, "reading uncompressed data ('%s')", filename);
			goto return_failure;
		}
	}

	// Bye!
	return image;

return_failure:
	if (image != NULL)
		ImageDelete(image);

	return NULL;
}


/*-----------------------------

 ImageExLoadSgi()
-----------------------------*/
int ImageExLoadSgi(FILE* file, struct ImageEx* out, struct Status* st)
{
	struct SgiHead head;
	enum Endianness sys_endianness = EndianSystem();

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	// Head
	if (fread(&head, sizeof(struct SgiHead), 1, file) != 1)
	{
		StatusSet(st, "ImageExLoadSgi", STATUS_UNEXPECTED_EOF, "near head");
		return 1;
	}

	out->width = EndianTo_16(head.x_size, ENDIAN_BIG, sys_endianness);
	out->height = EndianTo_16(head.y_size, ENDIAN_BIG, sys_endianness);
	out->data_offset = 512;
	out->endianness = ENDIAN_BIG;
	out->compression = (head.compression == 0) ? IMAGE_UNCOMPRESSED_PLANAR : IMAGE_SGI_RLE;

	head.z_size = EndianTo_16(head.z_size, ENDIAN_BIG, sys_endianness);

	switch (EndianTo_32(head.pixel_type, ENDIAN_BIG, sys_endianness))
	{
	case 1: StatusSet(st, "ImageExLoadSgi", STATUS_OBSOLETE_FEATURE, "dithered image"); return 1;
	case 2: StatusSet(st, "ImageExLoadSgi", STATUS_OBSOLETE_FEATURE, "indexed image"); return 1;
	case 3: StatusSet(st, "ImageExLoadSgi", STATUS_OBSOLETE_FEATURE, "palette data"); return 1;
	case 0: break;
	}

	switch (EndianTo_16(head.dimension, ENDIAN_BIG, sys_endianness))
	{
	case 1: // One dimensional grayscale image (only uses width/x_size)
		out->height = 1;
		head.z_size = 1;
		break;

	case 2: // Two dimensional grayscale image (uses width/x and height/y_size)
		head.z_size = 1;
		break;

		// case 3: // We converted the previous two into this case (that uses all x, y and z_size)
		// From here we can ignore the 'dimension' field
	}

	if (head.z_size > 4)
	{
		StatusSet(st, "ImageExLoadSgi", STATUS_UNSUPPORTED_FEATURE, "number of channels (%i)", head.z_size);
		return 1;
	}

	if (head.precision == 1)
	{
		// 8 bits per component image
		switch (head.z_size)
		{
		case 1: out->format = IMAGE_GRAY8; break;
		case 2: out->format = IMAGE_GRAYA8; break;
		case 3: out->format = IMAGE_RGB8; break;
		case 4: out->format = IMAGE_RGBA8; break;
		}

		out->uncompressed_size = ImageBpp(out->format) * out->width * out->height;
	}
	else
	{
		StatusSet(st, "ImageExLoadSgi", STATUS_UNSUPPORTED_FEATURE, "precision (%i)", head.precision);
		return 1;
	}

	return 0;
}


/*-----------------------------

 ImageSaveSgi()
-----------------------------*/
EXPORT struct Status ImageSaveSgi(struct Image* image, const char* filename)
{
	struct Status st = {.code = STATUS_SUCCESS};
	struct SgiHead head = {0};
	FILE* file = NULL;
	enum Endianness sys_endianness = EndianSystem();

	if (image->width > UINT16_MAX || image->height > UINT16_MAX)
	{
		StatusSet(&st, "ImageSaveSgi", STATUS_UNSUPPORTED_FEATURE, "image dimensions ('%s')", filename);
		return st;
	}

	if (image->format != IMAGE_GRAY8 && image->format != IMAGE_GRAYA8 && image->format != IMAGE_RGB8 &&
	    image->format != IMAGE_RGBA8)
	{
		StatusSet(&st, "ImageSaveSgi", STATUS_UNSUPPORTED_FEATURE, "image format ('%s')", filename);
		return st;
	}

	if ((file = fopen(filename, "wb")) == NULL)
	{
		StatusSet(&st, "ImageSaveSgi", STATUS_FS_ERROR, "'%s'", filename);
		return st;
	}

	// Head
	head.magic = EndianTo_16(SGI_MAGIC, sys_endianness, ENDIAN_BIG);
	head.compression = 0;
	head.precision = 1;
	head.dimension = EndianTo_16(3, sys_endianness, ENDIAN_BIG);

	head.x_size = EndianTo_16(image->width, sys_endianness, ENDIAN_BIG);
	head.y_size = EndianTo_16(image->height, sys_endianness, ENDIAN_BIG);
	head.z_size = EndianTo_16(Channels(image->format), sys_endianness, ENDIAN_BIG);

	head.pixel_type = 0;

	if (fwrite(&head, sizeof(struct SgiHead), 1, file) != 1)
	{
		StatusSet(&st, "ImageSaveSgi", STATUS_IO_ERROR, "head ('%s')", filename);
		goto return_failure;
	}

	// Data
	size_t bytes = ImageBpp(image->format);
	uint8_t* src = NULL;

	if (fseek(file, 512, SEEK_SET) != 0) // Data start at offset 512
	{
		StatusSet(&st, "ImageSaveSgi", STATUS_IO_ERROR, "at data seek ('%s')", filename);
		goto return_failure;
	}

	for (uint16_t ch = 0; ch < Channels(image->format); ch++)
	{
		// TODO: '__ssize_t' not a standard

		for (__ssize_t row = (image->height - 1); row >= 0; row--)
		{
			src = ((uint8_t*)image->data) + (image->size / image->height) * row + ch;

			for (size_t col = 0; col < image->width; col++)
			{
				if (fwrite(&src[col * bytes], 1, 1, file) != 1)
				{
					StatusSet(&st, "ImageSaveSgi", STATUS_IO_ERROR, "data ('%s')", filename);
					goto return_failure;
				}
			}
		}
	}

	// Bye!
	fclose(file);
	return st;

return_failure:
	fclose(file);
	return st;
}
