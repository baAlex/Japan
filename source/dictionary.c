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
 - Alexander Brandt 2019-2020

 https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 https://stackoverflow.com/a/30874878
 https://stackoverflow.com/a/29787467
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "japan-dictionary.h"


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
	struct jaDictionaryItem* item[BUCKET_DEPTH];
	struct Bucket* overflow_next;
};

struct CycleBucketState
{
	struct Bucket* bucket;
	size_t depth;

	struct Bucket* previous_bucket;
};

struct jaDictionary
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
static inline uint64_t sPow(uint64_t base, uint64_t exp)
{
	uint64_t result = 1;

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
static inline size_t sGetAddress(const struct jaDictionary* dictionary, const char* key, uint64_t* out_hash)
{
	uint64_t hash = FNV_OFFSET_BASIS;
	size_t address = 0;
	size_t size = 0;

	size = strlen(key);

	// FNV-1 hash
	if (dictionary->hash_function == NULL)
		hash = jaFNV1Hash(key, size);
	else
		hash = dictionary->hash_function(key, size);

	if (out_hash != NULL)
		*out_hash = hash;

	// Linear-hashing address
	address = (size_t)((hash % (INITIAL_BUCKETS * sPow(2, (uint64_t)dictionary->level))) % SIZE_MAX);

	if (address < dictionary->pointer)
		address = (size_t)((hash % (INITIAL_BUCKETS * sPow(2, (uint64_t)dictionary->level + 1))) % SIZE_MAX);

	return address;
}


/*-----------------------------

 sCycleBucket()
-----------------------------*/
static inline int sCycleBucket(struct CycleBucketState* state, struct jaDictionaryItem*** out)
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
static int sLocateInBucket(struct jaDictionary* dictionary, struct jaDictionaryItem* item, size_t address)
{
	struct CycleBucketState state = {0};
	struct jaDictionaryItem** item_slot = NULL;

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
static int sResize(struct jaDictionary* dictionary, enum ResizeDirection direction)
{
	struct jaDictionaryItem** item_slot = NULL;
	struct CycleBucketState state = {0};

	struct jaDictionaryItem* item = NULL;
	size_t address = 0;

	size_t to_rehash = dictionary->pointer;
	void* old_ptr = NULL;

	// Update counters
	if (direction == RESIZE_GROWN)
	{
		dictionary->buckets_no += 1;

		if (dictionary->pointer < (size_t)((INITIAL_BUCKETS * sPow(2, (uint64_t)dictionary->level)) % SIZE_MAX))
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
			dictionary->pointer = (size_t)((INITIAL_BUCKETS * sPow(2, dictionary->level)) % SIZE_MAX);
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
				JA_DEBUG_PRINT(" - Rehashing '%s', address: %03zu -> %03zu\n", item->key, to_rehash, address);
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

	JA_DEBUG_PRINT(" - Buckets: %zu (p: %zu)\n", dictionary->buckets_no, dictionary->pointer);
	return 0;
}


/*-----------------------------

 jaFNV1Hash()
-----------------------------*/
uint64_t jaFNV1Hash(const char* key, size_t size)
{
	uint64_t hash = FNV_OFFSET_BASIS;

	for (size_t i = 0; i < size; i++)
	{
		hash = hash ^ (uint64_t)key[i];
		hash = hash * FNV_PRIME;
	}

	return hash;
}


/*-----------------------------

 jaDictionaryCreate()
-----------------------------*/
struct jaDictionary* jaDictionaryCreate(uint64_t (*hash_function)(const char*, size_t))
{
	struct jaDictionary* dictionary = NULL;

	if ((dictionary = malloc(sizeof(struct jaDictionary))) != NULL)
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

 jaDictionaryDelete()
-----------------------------*/
void jaDictionaryDelete(struct jaDictionary* dictionary)
{
	struct CycleBucketState state = {0};
	struct jaDictionaryItem** item_slot = NULL;

	if (dictionary != NULL)
	{
		for (size_t i = 0; i < dictionary->buckets_no; i++)
		{
			state.bucket = &dictionary->buckets[i];
			state.previous_bucket = NULL;

			while (sCycleBucket(&state, &item_slot) != 1)
			{
				if (*item_slot != NULL)
				{
					if ((*item_slot)->callback_delete != NULL)
						(*item_slot)->callback_delete(*item_slot);

					free(*item_slot);
				}

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

 jaDictionaryAdd()
-----------------------------*/
struct jaDictionaryItem* jaDictionaryAdd(struct jaDictionary* dictionary, const char* key, void* data, size_t data_size)
{
	struct jaDictionaryItem* item = NULL;
	size_t key_size = 0;

	if (dictionary == NULL || key == NULL)
		return NULL;

	key_size = strlen(key) + 1;

	if ((item = malloc(sizeof(struct jaDictionaryItem) + key_size + data_size)) != NULL)
	{
		item->dictionary = dictionary;
		item->callback_delete = NULL;

		strncpy(item->key, key, key_size);

		if (data_size == 0)
			item->data = data;
		else
		{
			item->data = (void*)((struct jaDictionaryItem*)item + 1);
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

	JA_DEBUG_PRINT("(jaDictionaryAdd) key: '%s', address: %03zu, hash: 0x%016lX\n", key, address, hash);
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

 jaDictionaryGet()
-----------------------------*/
struct jaDictionaryItem* jaDictionaryGet(const struct jaDictionary* dictionary, const char* key)
{
	struct CycleBucketState state = {0};
	struct jaDictionaryItem** item_slot = NULL;

	if (dictionary != NULL && key != NULL)
	{
		uint64_t hash = 0;
		size_t address = sGetAddress(dictionary, key, &hash);

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

 jaDictionaryRemove()
-----------------------------*/
inline void jaDictionaryRemove(struct jaDictionaryItem* item)
{
	if (jaDictionaryDetach(item) == 0)
	{
		if (item->callback_delete != NULL)
			item->callback_delete(item);

		free(item);
	}
}


/*-----------------------------

 jaDictionaryDetach()
-----------------------------*/
int jaDictionaryDetach(struct jaDictionaryItem* item)
{
	struct CycleBucketState state = {0};
	struct jaDictionaryItem** item_slot = NULL;
	struct jaDictionary* d = NULL;

	if (item != NULL)
	{
		uint64_t hash = 0;
		size_t address = sGetAddress(item->dictionary, item->key, &hash);

		JA_DEBUG_PRINT("(jaDictionaryDetach) key: '%s', address: %03zu, hash: 0x%016lX\n", item->key, address, hash);

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

 jaDictionaryIterate()
-----------------------------*/
void jaDictionaryIterate(struct jaDictionary* dictionary, void (*callback)(struct jaDictionaryItem*, void*),
                         void* extra_data)
{
	struct CycleBucketState state = {0};
	struct jaDictionaryItem** item_slot = NULL;

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
