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
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
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

	DLL_EXP struct Dictionary* DictionaryCreate(uint64_t (*hash_function)(const char*, size_t));
	DLL_EXP void DictionaryDelete(struct Dictionary* dictionary);

	DLL_EXP struct DictionaryItem* DictionaryAdd(struct Dictionary* dictionary, const char* key, void* data, size_t data_size);
	DLL_EXP struct DictionaryItem* DictionaryGet(const struct Dictionary* dictionary, const char* key);

	DLL_EXP void DictionaryRemove(struct DictionaryItem* item);
	DLL_EXP int DictionaryDetach(struct DictionaryItem* item);

	DLL_EXP void DictionaryIterate(struct Dictionary*, void (*callback)(struct DictionaryItem*, void*), void* extra_data);

#endif
