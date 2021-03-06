/*-----------------------------

 [japan-image.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_IMAGE_H
#define JAPAN_IMAGE_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stddef.h>
#include <stdio.h>

#include "japan-endianness.h"
#include "japan-status.h"

enum jaImageStorage
{
	JA_IMAGE_UNCOMPRESSED_INTERLEAVED = 0, // (r, g, b), (r, g, b), (r, g, b)...
	JA_IMAGE_UNCOMPRESSED_PLANAR,          // (r, r, r...), (g, g, g...), (b, b, b...)
	JA_IMAGE_SGI_RLE
};

enum jaImageFormat
{
	JA_IMAGE_U8 = 0,
	JA_IMAGE_U16,
	JA_IMAGE_FLOAT
};

struct jaImage
{
	size_t width;
	size_t height;
	size_t channels;

	enum jaImageFormat format;
	size_t size; // In bytes

	void* data;
};

struct jaImageEx
{
	size_t width;  // In pixels
	size_t height; // "
	size_t channels;

	enum jaImageFormat format;
	enum jaEndianness endianness;
	enum jaImageStorage storage;

	size_t uncompressed_size;

	size_t data_offset;
};

JA_EXPORT struct jaImage* jaImageCreate(enum jaImageFormat, size_t width, size_t height, size_t channels);
JA_EXPORT void jaImageDelete(struct jaImage* image);

JA_EXPORT struct jaImage* jaImageLoad(const char* filename, struct jaStatus*);
JA_EXPORT int jaImageSaveSgi(const struct jaImage* image, const char* filename, struct jaStatus*);
JA_EXPORT int jaImageSaveRaw(const struct jaImage* image, const char* filename, struct jaStatus*);

JA_EXPORT int jaImageExLoad(FILE* file, struct jaImageEx* out, struct jaStatus*);

JA_EXPORT int jaBytesPerPixel(const struct jaImage* image);
JA_EXPORT int jaBitsPerComponent(enum jaImageFormat);

#endif
