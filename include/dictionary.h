/*-----------------------------

 [dictionary.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef DICTIONARY_H
#define DICTIONARY_H

	#include <stddef.h>
	#include <stdbool.h>

	struct Dictionary;

	struct DictionaryItem
	{
		struct Dictionary* dictionary;

		void* data;
		char key[];
	};

	struct Dictionary* DictionaryCreate();
	void DictionaryDelete(struct Dictionary* dictionary);

	struct DictionaryItem* DictionaryAdd(struct Dictionary* dictionary, const char* key, void* data, size_t data_size);
	struct DictionaryItem* DictionaryGet(const struct Dictionary* dictionary, const char* key);

	// void DictionaryRemove(struct DictionaryItem* item);
	// int DictionaryDetach(struct DictionaryItem* item);

	// struct DictionaryItem* DictionaryIterate(const struct Dictionary* dictionary, struct DictionaryState* state);

#endif