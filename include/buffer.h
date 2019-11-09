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
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
#endif

#ifndef BUFFER_H
#define BUFFER_H

	#include <stddef.h>

	struct Buffer
	{
		void* data;
		size_t size;
	};

	DLL_EXP void BufferClean(struct Buffer* buffer);
	DLL_EXP void* BufferResize(struct Buffer* buffer, size_t new_size);
	DLL_EXP void* BufferResizeZero(struct Buffer* buffer, size_t new_size);

#endif
