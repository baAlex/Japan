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
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
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

	DLL_EXP void ListClean(struct List* list);

	DLL_EXP struct ListItem* ListAdd(struct List* list, void* data, size_t data_size);
	DLL_EXP struct ListItem* ListAddAfter(struct ListItem* item, void* data, size_t data_size);
	DLL_EXP struct ListItem* ListAddBefore(struct ListItem* item, void* data, size_t data_size);

	DLL_EXP void ListRemove(struct ListItem* item);
	DLL_EXP int ListDetach(struct ListItem* item);

	DLL_EXP struct ListItem* ListIterate(struct ListState* state);

#endif
