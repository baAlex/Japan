/*-----------------------------

 [japan-buffer.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_BUFFER_H
#define JAPAN_BUFFER_H

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

#include <stddef.h>

struct jaBuffer
{
	void* data;
	size_t size;
};

JA_EXPORT void jaBufferClean(struct jaBuffer* buffer);
JA_EXPORT void* jaBufferResize(struct jaBuffer* buffer, size_t new_size);
JA_EXPORT void* jaBufferResizeZero(struct jaBuffer* buffer, size_t new_size);

#endif
