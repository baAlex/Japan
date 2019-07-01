/*-----------------------------

 [tree.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.0"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 0
#endif

#ifndef TREE_H
#define TREE_H

	#include <stddef.h>
	#include "buffer.h"

	#define TREE_STACK_DEPTH 50

	struct Tree
	{
		struct Tree* parent;
		struct Tree* next;
		struct Tree* previous;
		struct Tree* children;
		struct Tree* last_children;

		void (*callback_delete)(void*);

		void* data;
	};

	struct TreeState
	{
		struct Tree* start; // Set before iterate

		struct Tree* actual;
		size_t depth;

		struct Tree* future_return;
		size_t future_depth;

		struct Tree* future_parent[TREE_STACK_DEPTH];
	};

	struct Tree* TreeCreate(struct Tree* parent, void* data, size_t data_size);
	void TreeDelete(struct Tree* tree);

	struct Tree* TreeIterate(struct TreeState* state, struct Buffer* buffer);

	int TreeAttach(struct Tree* tree, struct Tree* new_parent);
	int TreeDetach(struct Tree* tree);

#endif
