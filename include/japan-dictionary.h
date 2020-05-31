/*-----------------------------

 [japan-dictionary.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_DICTIONARY_H
#define JAPAN_DICTIONARY_H

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
#include <stdint.h>

struct jaDictionary;

struct jaDictionaryItem
{
	struct jaDictionary* dictionary;

	void (*callback_delete)(struct jaDictionaryItem*);

	void* data;
	char key[];
};

JA_EXPORT uint64_t jaFNV1Hash(const char* key, size_t size);

JA_EXPORT struct jaDictionary* jaDictionaryCreate(uint64_t (*hash_function)(const char*, size_t));
JA_EXPORT void jaDictionaryDelete(struct jaDictionary* dictionary);

JA_EXPORT struct jaDictionaryItem* jaDictionaryAdd(struct jaDictionary* dictionary, const char* key, void* data,
                                                   size_t data_size);
JA_EXPORT struct jaDictionaryItem* jaDictionaryGet(const struct jaDictionary* dictionary, const char* key);

JA_EXPORT void jaDictionaryRemove(struct jaDictionaryItem* item);
JA_EXPORT int jaDictionaryDetach(struct jaDictionaryItem* item);

JA_EXPORT void jaDictionaryIterate(struct jaDictionary*, void (*callback)(struct jaDictionaryItem*, void*),
                                   void* extra_data);

#endif
