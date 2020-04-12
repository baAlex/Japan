/*-----------------------------

 [japan-tree.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_TREE_H
#define JAPAN_TREE_H

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

#include <stddef.h>
#include "japan-buffer.h"

#define JA_TREE_STACK_DEPTH 50

struct jaTree
{
	struct jaTree* parent;
	struct jaTree* next;
	struct jaTree* previous;
	struct jaTree* children;
	struct jaTree* last_children;

	void (*callback_delete)(struct jaTree*);

	void* data;
};

struct jaTreeState
{
	struct jaTree* start; // Set before iterate

	struct jaTree* actual;
	size_t depth;

	struct jaTree* future_return;
	size_t future_depth;

	struct jaTree* future_parent[JA_TREE_STACK_DEPTH];
};

JA_EXPORT struct jaTree* jaTreeCreate(struct jaTree* parent, void* data, size_t data_size);
JA_EXPORT void jaTreeDelete(struct jaTree* tree);

JA_EXPORT struct jaTree* jaTreeIterate(struct jaTreeState* state, struct jaBuffer* buffer);

JA_EXPORT int jaTreeAttach(struct jaTree* tree, struct jaTree* new_parent);
JA_EXPORT int jaTreeDetach(struct jaTree* tree);

#endif
