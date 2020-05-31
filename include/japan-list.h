/*-----------------------------

 [japan-list.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_LIST_H
#define JAPAN_LIST_H

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
