/*-----------------------------

 [lists.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

#include "japan-list.h"


/*-----------------------------

 ListTest1_Iteration()
-----------------------------*/
#define BEST_TOP1 "1. On Her Majesty's Secret Service"
#define BEST_TOP2 "2. You Only Live Twice"
#define BEST_TOP3 "3. The Living Daylights"

void ListTest1_Iteration(void** cmocka_state)
{
	(void)cmocka_state;

	// Create a list
	struct jaListItem* item = NULL;
	struct jaList list = {0};

	item = jaListAdd(&list, NULL, 0);
	item->data = BEST_TOP1;

	item = jaListAdd(&list, NULL, 0);
	item->data = BEST_TOP2;

	item = jaListAdd(&list, NULL, 0);
	item->data = BEST_TOP3;

	// Iterate it
	struct jaListState state = {0};
	int i = 0;

	state.start = list.first;

	while ((item = jaListIterate(&state)) != NULL)
	{
		printf(" - %s\n", (char*)item->data);

		// Assert correct order
		switch (i)
		{
		case 0: assert_string_equal(item->data, BEST_TOP1); break;
		case 1: assert_string_equal(item->data, BEST_TOP2); break;
		case 2: assert_string_equal(item->data, BEST_TOP3);
		};

		i++;
	}

	// Now in reverse
	i = 0;
	state.start = list.last;
	state.reverse = true;

	while ((item = jaListIterate(&state)) != NULL)
	{
		switch (i)
		{
		case 0: assert_string_equal(item->data, BEST_TOP3); break;
		case 1: assert_string_equal(item->data, BEST_TOP2); break;
		case 2: assert_string_equal(item->data, BEST_TOP1);
		};

		i++;
	}

	// Bye!
	jaListClean(&list);
}


/*-----------------------------

 ListTest2_RemovalAtIteration()
-----------------------------*/
#define WORST_TOP1 "1. Die Another Day"
#define WORST_TOP2 "2. Diamonds Are Forever"
#define WORST_TOP3 "3. Those with Roger Moore"

void ListTest2_RemovalAtIteration(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaListItem* item = NULL;
	struct jaList list = {0};

	// If size == 0, data is assigned not copied
	item = jaListAdd(&list, WORST_TOP1, 0);
	item = jaListAdd(&list, WORST_TOP2, 0);
	item = jaListAdd(&list, WORST_TOP3, 0);

	// Iterate it
	struct jaListState state = {0};
	int i = 0;

	state.start = list.last;
	state.reverse = true;

	while ((item = jaListIterate(&state)) != NULL)
	{
		printf(" - %s\n", (char*)item->data);

		switch (i)
		{
		case 0: assert_string_equal(item->data, WORST_TOP3); break;
		case 1: assert_string_equal(item->data, WORST_TOP2); break;
		case 2: assert_string_equal(item->data, WORST_TOP1);
		};

		// And because they are bad, lets delete them
		jaListRemove(item);

		i++;
	}

	// Iterate it again, for tests purposes
	state.start = list.first;
	i = 0;

	while ((item = jaListIterate(&state)) != NULL)
		i++;

	assert_int_equal(i, 0); // Nothing iterated

	// Add new items to the "dirty" list
	item = jaListAdd(&list, NULL, 0);
	item->data = WORST_TOP1;

	item = jaListAdd(&list, NULL, 0);
	item->data = WORST_TOP2;

	// Iterate in a simple (hacky) way
	i = 0;

	for (item = list.first; item != NULL; item = item->next)
	{
		switch (i)
		{
		case 0: assert_string_equal(item->data, WORST_TOP1); break;
		case 1: assert_string_equal(item->data, WORST_TOP2);
		};

		i++;
	}

	jaListRemove(list.first);

	// Bye!
	jaListClean(&list);
}


/*-----------------------------

 ListTest3_BeforeAfterAddition()
-----------------------------*/
struct Quote
{
	char str[64];
};

void ListTest3_BeforeAfterAddition(void** cmocka_state)
{
	(void)cmocka_state;
	struct jaList list = {0};

	// Items using AddAfter() and AddBefore()
	struct jaListItem* iexpect = jaListAdd(&list, NULL, sizeof(struct Quote));
	strncpy(((struct Quote*)iexpect->data)->str, "I expect ", 64);

	struct jaListItem* you = jaListAdd(&list, NULL, sizeof(struct Quote));
	strncpy(((struct Quote*)you->data)->str, "you ", 64);

	struct jaListItem* mrbond = jaListAddBefore(iexpect, NULL, sizeof(struct Quote));
	strncpy(((struct Quote*)mrbond->data)->str, "Mr Bond, ", 64);

	struct jaListItem* no = jaListAddBefore(mrbond, NULL, sizeof(struct Quote));
	strncpy(((struct Quote*)no->data)->str, "No, ", 64);

	struct jaListItem* todie = jaListAddAfter(list.last, NULL, sizeof(struct Quote));
	strncpy(((struct Quote*)todie->data)->str, "to die", 64);

	int i = 0;

	for (struct jaListItem* item = list.first; item != NULL; item = item->next)
	{
		printf("%s", ((struct Quote*)item->data)->str);

		switch (i)
		{
		case 0: assert_string_equal(((struct Quote*)item->data)->str, "No, "); break;
		case 1: assert_string_equal(((struct Quote*)item->data)->str, "Mr Bond, "); break;
		case 2: assert_string_equal(((struct Quote*)item->data)->str, "I expect "); break;
		case 3: assert_string_equal(((struct Quote*)item->data)->str, "you "); break;
		case 4: assert_string_equal(((struct Quote*)item->data)->str, "to die");
		};

		i++;
	}

	printf("\n");

	// Detach items
	jaListDetach(no);
	jaListDetach(you);

	i = 0;

	for (struct jaListItem* item = list.first; item != NULL; item = item->next)
	{
		printf("%s", ((struct Quote*)item->data)->str);

		switch (i)
		{
		case 0: assert_string_equal(((struct Quote*)item->data)->str, "Mr Bond, "); break;
		case 1: assert_string_equal(((struct Quote*)item->data)->str, "I expect "); break;
		case 2: assert_string_equal(((struct Quote*)item->data)->str, "to die");
		};

		i++;
	}

	printf("\n");

	jaListRemove(no);
	jaListRemove(you);

	// Bye!
	jaListClean(&list);
}
