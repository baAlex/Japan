/*-----------------------------

 [japan-list.h]
 - Alexander Brandt 2019-2020
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


#ifndef JA_LIST_H
#define JA_LIST_H

#include <stddef.h>
#include <stdbool.h>

struct jaListItem;

struct jaList
{
	struct jaListItem* first;
	struct jaListItem* last;
	size_t items_no;
};

struct jaListItem
{
	struct jaList* list;
	struct jaListItem* next;
	struct jaListItem* previous;

	void (*callback_delete)(struct jaListItem*);

	void* data;
};

struct jaListState
{
	struct jaListItem* start; // Set before iterate
	bool reverse;             // Set before iterate

	struct jaListItem* actual;
	struct jaListItem* future_return;
};

JA_EXPORT void jaListClean(struct jaList* list);

JA_EXPORT struct jaListItem* jaListAdd(struct jaList* list, void* data, size_t data_size);
JA_EXPORT struct jaListItem* jaListAddAfter(struct jaListItem* item, void* data, size_t data_size);
JA_EXPORT struct jaListItem* jaListAddBefore(struct jaListItem* item, void* data, size_t data_size);

JA_EXPORT void jaListRemove(struct jaListItem* item);
JA_EXPORT int jaListDetach(struct jaListItem* item);

JA_EXPORT struct jaListItem* jaListIterate(struct jaListState* state);

#endif
