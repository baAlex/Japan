/*-----------------------------

 [image.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.1"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 1
#endif

#ifndef IMAGE_H
#define IMAGE_H

	#include <stdio.h>
	#include <stddef.h>

	#include "endianness.h"
	#include "status.h"

	enum ImageCompression
	{
		IMAGE_UNCOMPRESSED_INTERLEAVED = 0, // (r, g, b), (r, g, b), (r, g, b)...
		IMAGE_UNCOMPRESSED_PLANAR,          // (r, r, r...), (g, g, g...), (b, b, b...)
		IMAGE_SGI_RLE
	};

	enum ImageFormat
	{
		IMAGE_GRAY8 = 0,
		IMAGE_GRAYA8,
		IMAGE_RGB8,
		IMAGE_RGBA8,
		IMAGE_GRAY16,
		IMAGE_GRAYA16,
		IMAGE_RGB16,
		IMAGE_RGBA16
	};

	struct Image
	{
		size_t width;
		size_t height;
		size_t size; // In bytes

		enum ImageFormat format;
		void* data;
	};

	struct ImageEx
	{
		size_t width;
		size_t height;
		size_t uncompressed_size; // In bytes

		enum Endianness endianness;
		enum ImageCompression compression;
		enum ImageFormat format;

		size_t data_offset;
	};

	struct Image* ImageCreate(enum ImageFormat, size_t width, size_t height);
	void ImageDelete(struct Image* image);

	struct Image* ImageLoad(const char* filename, struct Status*);
	struct Status ImageSaveSgi(struct Image* image, const char* filename);
	struct Status ImageSaveRaw(struct Image* image, const char* filename);

	int ImageExLoad(FILE* file, struct ImageEx* out, struct Status*);

	int ImageBytesPerPixel(enum ImageFormat);
	int ImageBitsPerComponent(enum ImageFormat);
	int ImageChannels(enum ImageFormat);

#endif
