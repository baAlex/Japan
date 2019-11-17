/*-----------------------------

 [dictionary.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
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

	JAPAN_API struct Dictionary* DictionaryCreate(uint64_t (*hash_function)(const char*, size_t));
	JAPAN_API void DictionaryDelete(struct Dictionary* dictionary);

	JAPAN_API struct DictionaryItem* DictionaryAdd(struct Dictionary* dictionary, const char* key, void* data, size_t data_size);
	JAPAN_API struct DictionaryItem* DictionaryGet(const struct Dictionary* dictionary, const char* key);

	JAPAN_API void DictionaryRemove(struct DictionaryItem* item);
	JAPAN_API int DictionaryDetach(struct DictionaryItem* item);

	JAPAN_API void DictionaryIterate(struct Dictionary*, void (*callback)(struct DictionaryItem*, void*), void* extra_data);

#endif
