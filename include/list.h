/*-----------------------------

 [list.h]
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

#ifndef LIST_H
#define LIST_H

	#include <stddef.h>
	#include <stdbool.h>

	struct ListItem;

	struct List
	{
		struct ListItem* first;
		struct ListItem* last;
		size_t items_no;
	};

	struct ListItem
	{
		struct List* list;
		struct ListItem* next;
		struct ListItem* previous;

		void (*callback_delete)(void*);

		void* data;
	};

	struct ListState
	{
		struct ListItem* start; // Set before iterate
		bool reverse;           // Set before iterate

		struct ListItem* actual;
		struct ListItem* future_return;
	};

	JAPAN_API void ListClean(struct List* list);

	JAPAN_API struct ListItem* ListAdd(struct List* list, void* data, size_t data_size);
	JAPAN_API struct ListItem* ListAddAfter(struct ListItem* item, void* data, size_t data_size);
	JAPAN_API struct ListItem* ListAddBefore(struct ListItem* item, void* data, size_t data_size);

	JAPAN_API void ListRemove(struct ListItem* item);
	JAPAN_API int ListDetach(struct ListItem* item);

	JAPAN_API struct ListItem* ListIterate(struct ListState* state);

#endif
