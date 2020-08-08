/*-----------------------------

 [japan-buffer.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_BUFFER_H
#define JAPAN_BUFFER_H

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
JA_EXPORT int jaBufferCopy(struct jaBuffer* dest, const struct jaBuffer* org);
JA_EXPORT void* jaBufferResize(struct jaBuffer* buffer, size_t new_size);
JA_EXPORT void* jaBufferResizeZero(struct jaBuffer* buffer, size_t new_size);

#endif
