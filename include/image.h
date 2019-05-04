/*-----------------------------

 [image.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef IMAGE_H
#define IMAGE_H

	#include <stddef.h>
	#include "error.h"

	enum ImageFormat
	{
		IMAGE_GRAY8 = 0,
		IMAGE_GRAYA8,
		IMAGE_RGB8,
		IMAGE_RGBA8,
		IMAGE_GRAY16,  // TODO: Sgi support
		IMAGE_GRAYA16, // "
		IMAGE_RGB16,   // "
		IMAGE_RGBA16   // "
	};

	struct Image
	{
		size_t width;
		size_t height;
		size_t size; // In bytes

		enum ImageFormat format;
		void* data;
	};

	struct Image* ImageCreate(enum ImageFormat, size_t width, size_t height);
	void ImageDelete(struct Image* image);

	struct Image* ImageLoad(const char* filename, struct Error*);
	struct Error ImageSaveSgi(struct Image* image, const char* filename);
	// struct Error ImageSaveBmp(struct Image* image, const char* filename);
	struct Error ImageSaveRaw(struct Image* image, const char* filename);

#endif
