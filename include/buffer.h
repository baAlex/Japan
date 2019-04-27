/*-----------------------------

 [buffer.h]
 - Alexander Brandt 2019
-----------------------------*/

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
