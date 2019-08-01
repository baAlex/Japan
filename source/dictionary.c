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

 [dictionary.c]
 - Alexander Brandt 2019

 https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 https://stackoverflow.com/a/30874878
 https://stackoverflow.com/a/29787467
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) // Whitespace
#endif

#ifdef EXPORT_SYMBOLS
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT // Whitespace
#endif


#define INITIAL_BUCKETS 8
#define BUCKET_DEPTH 2
#define GROWN_THRESHOLD 75
#define SHRINK_THRESHOLD 40

#define FNV_OFFSET_BASIS 0xCBF29CE484222325
#define FNV_PRIME 0x100000001B3


enum ResizeDirection
{
	RESIZE_GROWN,
	RESIZE_SHRINK
};

struct Bucket
{
	struct DictionaryItem* item[BUCKET_DEPTH];
	struct Bucket* overflow_next;
};

struct CycleBucketState
{
	struct Bucket* bucket;
	size_t depth;

	struct Bucket* previous_bucket;
};

struct Dictionary
{
	size_t level;
	size_t pointer;
	size_t buckets_no;
	size_t items_no;

	struct Bucket* buckets;

	uint64_t (*hash_function)(const char*, size_t);
};


/*-----------------------------

 sPow()
-----------------------------*/
static inline int sPow(int base, int exp)
{
	int result = 1;

	while (exp != 0)
	{
		if (exp & 1)
			result *= base;

		exp /= 2;
		base *= base;
	}

	return result;
}


/*-----------------------------

 sGetAddress()
-----------------------------*/
static inline size_t sGetAddress(const struct Dictionary* dictionary, const char* key, uint64_t* out_hash)
{
	uint64_t hash = FNV_OFFSET_BASIS;
	size_t address = 0;
	size_t size = 0;

	size = strlen(key) + 1;

	// FNV-1 hash
	if (dictionary->hash_function == NULL)
	{
		for (size_t i = 0; i < size; i++)
		{
			hash = hash ^ key[i];
			hash = hash * FNV_PRIME;
		}
	}
	else
		hash = dictionary->hash_function(key, size);

	// Linear-hashing address
	address = hash % (INITIAL_BUCKETS * sPow(2, dictionary->level));

	if (address < dictionary->pointer)
		address = hash % (INITIAL_BUCKETS * sPow(2, dictionary->level + 1));

	if (out_hash != NULL)
		*out_hash = hash;

	return address;
}


/*-----------------------------

 sCycleBucket()
-----------------------------*/
static inline int sCycleBucket(struct CycleBucketState* state, struct DictionaryItem*** out)
{
	if (state->bucket != NULL)
	{
		*out = &state->bucket->item[state->depth];
		state->depth += 1;

		if (state->depth == BUCKET_DEPTH)
		{
			state->depth = 0;
			state->previous_bucket = state->bucket;
			state->bucket = state->bucket->overflow_next;
		}

		return 0;
	}

	return 1;
}


/*-----------------------------

 sLocateInBucket()
-----------------------------*/
static int sLocateInBucket(struct Dictionary* dictionary, struct DictionaryItem* item, size_t address)
{
	struct CycleBucketState state = {0};
	struct DictionaryItem** item_slot = NULL;

	// Add item into a bucket
	state.bucket = &dictionary->buckets[address];

	while (sCycleBucket(&state, &item_slot) != 1)
	{
		if (item_slot != NULL && *item_slot == NULL)
		{
			*item_slot = item;
			break;
		}
	}

	// ... Into an overflow bucket
	if (item_slot == NULL || *item_slot != item)
	{
		struct Bucket* previous_bucket = state.previous_bucket;

		if (previous_bucket == NULL)
			previous_bucket = &dictionary->buckets[address];

		if ((previous_bucket->overflow_next = malloc(sizeof(struct Bucket))) != NULL)
		{
			memset(previous_bucket->overflow_next, 0, sizeof(struct Bucket));
			previous_bucket->overflow_next->item[0] = item;
		}
		else
			return 1;
	}

	return 0;
}


/*-----------------------------

 sResize()
-----------------------------*/
static int sResize(struct Dictionary* dictionary, enum ResizeDirection direction)
{
	struct DictionaryItem** item_slot = NULL;
	struct CycleBucketState state = {0};

	struct DictionaryItem* item = NULL;
	size_t address = 0;

	size_t to_rehash = dictionary->pointer;
	void* old_ptr = NULL;

	// Update counters
	if (direction == RESIZE_GROWN)
	{
		dictionary->buckets_no += 1;

		if (dictionary->pointer < (size_t)(INITIAL_BUCKETS * sPow(2, dictionary->level)))
			dictionary->pointer += 1;
		else
		{
			dictionary->pointer = 0;
			dictionary->level += 1;
		}

		// Grown
		old_ptr = dictionary->buckets;

		if ((dictionary->buckets = realloc(dictionary->buckets, dictionary->buckets_no * sizeof(struct Bucket))) ==
		    NULL)
		{
			dictionary->buckets = old_ptr;
			return 1;
		}

		memset((uint8_t*)dictionary->buckets + (dictionary->buckets_no - 1) * sizeof(struct Bucket), 0,
		       sizeof(struct Bucket));
	}
	else
	{
		dictionary->buckets_no -= 1;
		to_rehash = dictionary->buckets_no;

		if (dictionary->pointer > 0)
			dictionary->pointer -= 1;
		else
		{
			dictionary->pointer = (size_t)(INITIAL_BUCKETS * sPow(2, dictionary->level));
			dictionary->level -= 1;
		}
	}

	// Rehash pointed bucket
	state.bucket = &dictionary->buckets[to_rehash];
	state.depth = 0;

	while (sCycleBucket(&state, &item_slot) != 1)
	{
		if (*item_slot != NULL)
		{
			item = *item_slot;
			address = sGetAddress(dictionary, item->key, NULL);

			if (address == to_rehash) // New rehash produce the same address
				continue;

			// Relocate in a bucket
			if (sLocateInBucket(dictionary, item, address) == 0)
			{
				*item_slot = NULL;
				DEBUG_PRINT(" - Rehashing '%s', address: %03lu -> %03lu\n", item->key, to_rehash, address);
			}
			else
				return 1;
		}
	}

	// Shrink
	if (direction == RESIZE_SHRINK)
	{
		// Free overflow buckets (via a new iteration)
		state.bucket = &dictionary->buckets[to_rehash];
		state.depth = 0;

		while (sCycleBucket(&state, &item_slot) != 1)
		{
			if (state.depth == 0 && state.previous_bucket != &dictionary->buckets[to_rehash])
				free(state.previous_bucket);
		}

		// Actual resize
		old_ptr = dictionary->buckets;

		if ((dictionary->buckets = realloc(dictionary->buckets, dictionary->buckets_no * sizeof(struct Bucket))) ==
		    NULL)
		{
			// How many chances of this realloc() to fail exists?
			dictionary->buckets = old_ptr;
			return 1;
		}
	}

	DEBUG_PRINT(" - Buckets: %lu (p: %lu)\n", dictionary->buckets_no, dictionary->pointer);
	return 0;
}


/*-----------------------------

 DictionaryCreate()
-----------------------------*/
EXPORT struct Dictionary* DictionaryCreate(uint64_t (*hash_function)(const char*, size_t))
{
	struct Dictionary* dictionary = NULL;

	if ((dictionary = malloc(sizeof(struct Dictionary))) != NULL)
	{
		dictionary->level = 0;
		dictionary->pointer = 0;
		dictionary->buckets_no = INITIAL_BUCKETS;
		dictionary->items_no = 0;
		dictionary->hash_function = hash_function;

		if ((dictionary->buckets = calloc(dictionary->buckets_no, sizeof(struct Bucket))) == NULL)
		{
			free(dictionary);
			dictionary = NULL;
		}
	}

	return dictionary;
}


/*-----------------------------

 DictionaryDelete()
-----------------------------*/
EXPORT void DictionaryDelete(struct Dictionary* dictionary)
{
	struct CycleBucketState state = {0};
	struct DictionaryItem** item_slot = NULL;

	if (dictionary != NULL)
	{
		for (size_t i = 0; i < dictionary->buckets_no; i++)
		{
			state.bucket = &dictionary->buckets[i];
			state.previous_bucket = NULL;

			while (sCycleBucket(&state, &item_slot) != 1)
			{
				if (*item_slot != NULL)
					free(*item_slot);

				// Overflow buckets
				if (state.depth == 0 && state.previous_bucket != &dictionary->buckets[i])
					free(state.previous_bucket);
			}
		}

		free(dictionary->buckets);
		free(dictionary);
	}
}


/*-----------------------------

 DictionaryAdd()
-----------------------------*/
EXPORT struct DictionaryItem* DictionaryAdd(struct Dictionary* dictionary, const char* key, void* data,
                                            size_t data_size)
{
	struct DictionaryItem* item = NULL;
	size_t key_size = 0;

	if (dictionary == NULL || key == NULL)
		return NULL;

	key_size = strlen(key) + 1;

	if ((item = malloc(sizeof(struct DictionaryItem) + key_size + data_size)) != NULL)
	{
		item->dictionary = dictionary;
		strncpy(item->key, key, key_size);

		if (data_size == 0)
			item->data = data;
		else
		{
			item->data = (void*)((struct DictionaryItem*)item + 1);
			item->data = (void*)((uint8_t*)item->data + key_size);

			if (data != NULL)
				memcpy(item->data, data, data_size);
		}
	}
	else
		goto return_failure;

	// Add to a bucket
	uint64_t hash = 0;
	size_t address = sGetAddress(dictionary, key, &hash);

	if (sLocateInBucket(dictionary, item, address) != 0)
		goto return_failure;

	DEBUG_PRINT("(DictionaryAdd) key: '%s', address: %03lu, hash: 0x%016lX\n", key, address, hash);
	dictionary->items_no++;

	// Grown?
	if ((dictionary->items_no * 100) / (dictionary->buckets_no * BUCKET_DEPTH) > GROWN_THRESHOLD)
	{
		if (sResize(dictionary, RESIZE_GROWN) != 0)
			goto return_failure;
	}

	// Bye!
	return item;

return_failure:
	if (item != NULL)
		free(item);

	return NULL;
}


/*-----------------------------

 DictionaryGet()
-----------------------------*/
EXPORT struct DictionaryItem* DictionaryGet(const struct Dictionary* dictionary, const char* key)
{
	struct CycleBucketState state = {0};
	struct DictionaryItem** item_slot = NULL;

	if (dictionary != NULL && key != NULL)
	{
		uint64_t hash = 0;
		size_t address = sGetAddress(dictionary, key, &hash);

		DEBUG_PRINT("(DictionaryGet) key: '%s', address: %03lu, hash: 0x%016lX\n", key, address, hash);

		state.bucket = &dictionary->buckets[address];
		while (sCycleBucket(&state, &item_slot) != 1)
		{
			if (*item_slot != NULL && strcmp((*item_slot)->key, key) == 0)
				return *item_slot;
		}
	}

	return NULL;
}


/*-----------------------------

 DictionaryRemove()
-----------------------------*/
EXPORT inline void DictionaryRemove(struct DictionaryItem* item)
{
	if (DictionaryDetach(item) == 0)
		free(item);
}


/*-----------------------------

 DictionaryDetach()
-----------------------------*/
EXPORT int DictionaryDetach(struct DictionaryItem* item)
{
	struct CycleBucketState state = {0};
	struct DictionaryItem** item_slot = NULL;
	struct Dictionary* d = NULL;

	if (item != NULL)
	{
		uint64_t hash = 0;
		size_t address = sGetAddress(item->dictionary, item->key, &hash);

		DEBUG_PRINT("(DictionaryDetach) key: '%s', address: %03lu, hash: 0x%016lX\n", item->key, address, hash);

		state.bucket = &item->dictionary->buckets[address];
		while (sCycleBucket(&state, &item_slot) != 1)
		{
			if (*item_slot != NULL && strcmp((*item_slot)->key, item->key) == 0)
				*item_slot = NULL;
		}

		d = item->dictionary;

		item->dictionary->items_no -= 1;
		item->dictionary = NULL;

		// Shrink?
		if (d->buckets_no != INITIAL_BUCKETS && (d->items_no * 100) / (d->buckets_no * BUCKET_DEPTH) < SHRINK_THRESHOLD)
		{
			if (sResize(d, RESIZE_SHRINK) != 0)
				return 1;
		}

		return 0;
	}

	return 1;
}


/*-----------------------------

 DictionaryIterate()
-----------------------------*/
EXPORT void DictionaryIterate(struct Dictionary* dictionary, void (*callback)(struct DictionaryItem*, void*),
                              void* extra_data)
{
	struct CycleBucketState state = {0};
	struct DictionaryItem** item_slot = NULL;

	if (dictionary != NULL && callback != NULL)
	{
		for (size_t i = 0; i < dictionary->buckets_no; i++)
		{
			state.bucket = &dictionary->buckets[i];
			state.previous_bucket = NULL;

			while (sCycleBucket(&state, &item_slot) != 1)
			{
				if (*item_slot != NULL)
					callback(*item_slot, extra_data);
			}
		}
	}
}
