/*-----------------------------

 [dictionary.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.1"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 1
#endif

#ifndef DICTIONARY_H
#define DICTIONARY_H

	#include <stddef.h>
	#include <stdint.h>

	struct Dictionary;

	struct DictionaryItem
	{
		struct Dictionary* dictionary;

		void* data;
		char key[];
	};

	struct Dictionary* DictionaryCreate(uint64_t (*hash_function)(const char*, size_t));
	void DictionaryDelete(struct Dictionary* dictionary);

	struct DictionaryItem* DictionaryAdd(struct Dictionary* dictionary, const char* key, void* data, size_t data_size);
	struct DictionaryItem* DictionaryGet(const struct Dictionary* dictionary, const char* key);

	void DictionaryRemove(struct DictionaryItem* item);
	int DictionaryDetach(struct DictionaryItem* item);

	void DictionaryIterate(struct Dictionary*, void (*callback)(struct DictionaryItem*, void*), void* extra_data);

#endif
