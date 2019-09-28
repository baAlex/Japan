/*-----------------------------

 [buffer.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.2"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 2
#endif

#ifndef BUFFER_H
#define BUFFER_H

	#include <stddef.h>

	struct Buffer
	{
		void* data;
		size_t size;
	};

	void BufferClean(struct Buffer* buffer);
	void* BufferResize(struct Buffer* buffer, size_t new_size);
	void* BufferResizeZero(struct Buffer* buffer, size_t new_size);

#endif
