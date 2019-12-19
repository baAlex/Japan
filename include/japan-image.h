/*-----------------------------

 [japan-image.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif


#ifndef JA_IMAGE_H
#define JA_IMAGE_H

#include <stdio.h>
#include <stddef.h>

#include "japan-endianness.h"
#include "japan-status.h"

enum jaImageStorage
{
	IMAGE_UNCOMPRESSED_INTERLEAVED = 0, // (r, g, b), (r, g, b), (r, g, b)...
	IMAGE_UNCOMPRESSED_PLANAR,          // (r, r, r...), (g, g, g...), (b, b, b...)
	IMAGE_SGI_RLE
};

enum jaImageFormat
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

struct jaImage
{
	size_t width;
	size_t height;
	size_t size; // In bytes

	enum jaImageFormat format;
	void* data;
};

struct jaImageEx
{
	size_t width;  // In pixels
	size_t height; // In pixels, Ignored by ImageExLoadRaw() if set to 0

	size_t uncompressed_size; // Ignored by ImageExLoadRaw()

	enum jaEndianness endianness;
	enum jaImageStorage storage;
	enum jaImageFormat format;

	size_t data_offset;
};

JA_EXPORT struct jaImage* jaImageCreate(enum jaImageFormat, size_t width, size_t height);
JA_EXPORT void jaImageDelete(struct jaImage* image);

JA_EXPORT struct jaImage* jaImageLoad(const char* filename, struct jaStatus*);
JA_EXPORT int jaImageSaveSgi(const struct jaImage* image, const char* filename, struct jaStatus*);
JA_EXPORT int jaImageSaveRaw(const struct jaImage* image, const char* filename, struct jaStatus*);

JA_EXPORT struct jaImage* jaImageLoadRaw(FILE* file, const struct jaImageEx* in_ex, struct jaStatus*);
JA_EXPORT int jaImageExLoad(FILE* file, struct jaImageEx* out, struct jaStatus*);

JA_EXPORT int jaBytesPerPixel(enum jaImageFormat);
JA_EXPORT int jaBitsPerComponent(enum jaImageFormat);
JA_EXPORT int jaImageChannels(enum jaImageFormat);

#endif
