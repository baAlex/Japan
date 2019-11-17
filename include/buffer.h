/*-----------------------------

 [buffer.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
#endif

#ifndef BUFFER_H
#define BUFFER_H

	#include <stddef.h>

	struct Buffer
	{
		void* data;
		size_t size;
	};

	JAPAN_API void BufferClean(struct Buffer* buffer);
	JAPAN_API void* BufferResize(struct Buffer* buffer, size_t new_size);
	JAPAN_API void* BufferResizeZero(struct Buffer* buffer, size_t new_size);

#endif
