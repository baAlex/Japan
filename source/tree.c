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

 [tree.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "tree.h"

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


/*-----------------------------

 TreeIterate()
-----------------------------*/
EXPORT struct Tree* TreeIterate(struct TreeState* state, struct Buffer* buffer)
{
	struct Tree** future_parent_heap = NULL;

	if (state->start != NULL)
	{
		state->future_parent[0] = NULL;
		state->future_depth = 0;
		state->future_return = state->start;
		state->actual = state->start;
		state->start = NULL;
	}

	// Actual value (to return)
	if ((state->actual = state->future_return) == NULL)
		return NULL;

	state->depth = state->future_depth;
	state->future_return = NULL;

	// Future values
	{
		// Childrens
		if (state->actual->children != NULL)
		{
			if (state->depth < TREE_STACK_DEPTH)
			{
				state->future_parent[state->depth] = state->actual->next;
				state->future_return = state->actual->children;
			}
			else if (buffer != NULL)
			{
				if (BufferResize(buffer, state->depth * sizeof(void*)) == NULL)
					return NULL;

				future_parent_heap = buffer->data;
				future_parent_heap[state->depth] = state->actual->next;
				state->future_return = state->actual->children;
			}
			else
				state->future_return = NULL;

			state->future_depth++;
		}

		// Brothers
		else if (state->depth > 0 && state->actual->next != NULL)
		{
			state->future_return = state->actual->next;
		}

		// Next parent
		else if (state->depth > 0)
		{
			future_parent_heap = buffer->data;

			while ((state->future_depth -= 1) > 0)
			{
				if (state->future_depth < TREE_STACK_DEPTH)
				{
					if (state->future_parent[state->future_depth] != NULL)
					{
						state->future_return = state->future_parent[state->future_depth];
						break;
					}
				}
				else
				{
					if (future_parent_heap[state->future_depth] != NULL)
					{
						state->future_return = future_parent_heap[state->future_depth];
						break;
					}
				}
			}
		}
	}

	return state->actual;
}


/*-----------------------------

 TreeCreate()
-----------------------------*/
EXPORT struct Tree* TreeCreate(struct Tree* parent, void* data, size_t data_size)
{
	struct Tree* tree = NULL;

	if ((tree = malloc(sizeof(struct Tree) + data_size)) != NULL)
	{
		if (data_size == 0)
			tree->data = data;
		else
		{
			tree->data = (void*)((struct Tree*)tree + 1);

			if (data != NULL)
				memcpy(tree->data, data, data_size);
		}

		tree->callback_delete = NULL;
		tree->last_children = NULL;
		tree->children = NULL;
		tree->parent = NULL;
		tree->previous = NULL;
		tree->next = NULL;

		TreeAttach(tree, parent);
	}

	return tree;
}


/*-----------------------------

 TreeDelete()
-----------------------------*/
EXPORT void TreeDelete(struct Tree* tree)
{
	struct TreeState state;
	struct Buffer buffer = {0};

	if (tree != NULL)
	{
		state.start = tree;
		TreeDetach(tree);

		while ((tree = TreeIterate(&state, &buffer)) != NULL)
		{
			if (tree->callback_delete != NULL)
				tree->callback_delete(tree->data);

			free(tree);
		}

		BufferClean(&buffer);
	}
}


/*-----------------------------

 TreeAttach()
-----------------------------*/
EXPORT int TreeAttach(struct Tree* tree, struct Tree* new_parent)
{
	if (tree->parent == NULL && new_parent != NULL)
	{
		tree->parent = new_parent;

		// First children
		if (tree->parent->children == NULL)
		{
			tree->previous = NULL;
			tree->next = NULL;

			tree->parent->children = tree;
			tree->parent->last_children = tree;
		}

		// Last children
		else
		{
			tree->next = NULL;
			tree->previous = tree->parent->last_children;

			tree->parent->last_children->next = tree;
			tree->parent->last_children = tree;
		}

		return 0;
	}

	tree->previous = NULL;
	tree->next = NULL;

	return 1;
}


/*-----------------------------

 TreeDetach()
-----------------------------*/
EXPORT int TreeDetach(struct Tree* tree)
{
	if (tree->parent != NULL)
	{
		// First children
		if (tree->parent->children == tree)
		{
			tree->parent->children = tree->next;

			if (tree->next != NULL)
				tree->next->previous = NULL;
		}

		// Last children
		else if (tree->parent->last_children == tree)
		{
			tree->parent->last_children = tree->previous;
			tree->previous->next = NULL;
		}

		// Middle one
		else
		{
			tree->previous->next = tree->next;

			if (tree->next != NULL)
				tree->next->previous = tree->previous;
		}

		tree->parent = NULL;
		return 0;
	}

	return 1;
}
