/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [list.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "list.h"


/*-----------------------------

 sAllocItem()
-----------------------------*/
static struct ListItem* sAllocItem(struct List* list, void* data, size_t data_size)
{
	struct ListItem* item = NULL;

	if ((item = malloc(sizeof(struct ListItem) + data_size)) == NULL)
		return NULL;

	if (data_size == 0)
		item->data = data;
	else
	{
		item->data = (void*)((struct ListItem*)item + 1);

		if (data != NULL)
			memcpy(item->data, data, data_size);
	}

	item->list = list;
	item->next = NULL;
	item->previous = NULL;
	item->callback_delete = NULL;

	return item;
}


/*-----------------------------

 ListClean()
-----------------------------*/
void ListClean(struct List* list)
{
	while (list->last != NULL)
		ListRemove(list->last); // NOLINT(clang-analyzer-unix.Malloc)
}


/*-----------------------------

 ListAdd()
-----------------------------*/
struct ListItem* ListAdd(struct List* list, void* data, size_t data_size)
{
	struct ListItem* new_item = NULL;

	if ((new_item = sAllocItem(list, data, data_size)) == NULL)
		return NULL;

	if (list->last != NULL)
	{
		list->last->next = new_item;
		new_item->previous = list->last;
	}

	if (list->first == NULL)
		list->first = new_item;

	list->last = new_item;
	list->items_no += 1;

	return new_item;
}


/*-----------------------------

 ListAddAfter()
-----------------------------*/
struct ListItem* ListAddAfter(struct ListItem* item, void* data, size_t data_size)
{
	struct ListItem* new_item = NULL;

	if ((new_item = sAllocItem(item->list, data, data_size)) == NULL)
		return NULL;

	if (item->next == NULL)
	{
		item->next = new_item;
		new_item->previous = item;
		new_item->list->last = new_item;
	}
	else
	{
		new_item->previous = item;
		new_item->next = item->next;

		item->next->previous = new_item;
		item->next = new_item;
	}

	item->list->items_no += 1;

	return new_item;
}


/*-----------------------------

 ListAddBefore()
-----------------------------*/
struct ListItem* ListAddBefore(struct ListItem* item, void* data, size_t data_size)
{
	struct ListItem* new_item = NULL;

	if ((new_item = sAllocItem(item->list, data, data_size)) == NULL)
		return NULL;

	if (item->previous == NULL)
	{
		new_item->next = item;
		item->previous = new_item;
		item->list->first = new_item;
	}
	else
	{
		new_item->next = item;
		new_item->previous = item->previous;

		item->previous->next = new_item;
		item->previous = new_item;
	}

	item->list->items_no += 1;

	return new_item;
}


/*-----------------------------

 ListRemove()
-----------------------------*/
void ListRemove(struct ListItem* item)
{
	ListDetach(item);

	if (item->callback_delete != NULL)
		item->callback_delete(item);

	free(item);
}


/*-----------------------------

 ListDetach()
-----------------------------*/
int ListDetach(struct ListItem* item)
{
	if (item->list != NULL)
	{
		if (item->previous == NULL)
			item->list->first = item->next;
		else
			item->previous->next = item->next;

		if (item->next == NULL)
			item->list->last = item->previous;
		else
			item->next->previous = item->previous;

		item->list->items_no -= 1;

		item->next = NULL;
		item->previous = NULL;
		item->list = NULL;

		return 0;
	}

	return 1;
}


/*-----------------------------

 ListIterate()
-----------------------------*/
struct ListItem* ListIterate(struct ListState* state)
{
	struct ListItem* to_return = NULL;

	if (state->start != NULL)
	{
		to_return = state->start;

		state->future_return = state->start;
		state->start = NULL;
	}
	else
		to_return = state->future_return;

	if (state->reverse == false)
	{
		if (state->future_return != NULL)
			state->future_return = state->future_return->next;
	}

	else if (state->reverse == true)
	{
		if (state->future_return != NULL)
			state->future_return = state->future_return->previous;
	}

	state->actual = to_return;
	return to_return;
}
