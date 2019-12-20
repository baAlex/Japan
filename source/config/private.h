/*-----------------------------

 [config/private.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JA_CONFIG_PRIVATE_H
#define JA_CONFIG_PRIVATE_H

	#include <stdlib.h>
	#include <string.h>
	#include <stddef.h>
	#include <limits.h>
	#include <ctype.h>
	#include <math.h>

	#include "japan-dictionary.h"
	#include "japan-utilities.h"
	#include "japan-config.h"

	#include "../common.h"

	enum SetBy
	{
		SET_DEFAULT = 0,
		SET_BY_ARGUMENTS,
		SET_BY_FILE
	};

	enum Type
	{
		TYPE_INT = 0,
		TYPE_FLOAT,
		TYPE_STRING
	};

	union Value
	{
		int i;
		float f;
		const char* s;
	};

	struct Cvar
	{
		enum Type type;
		enum SetBy set_by;

		union Value value;
		union Value min;
		union Value max;
	};

	void PrintCallback(struct jaDictionaryItem* item, void* data);

	int StoreFloat(float* dest, const char* org, float min, float max);
	int StoreInt(int* dest, const char* org, int min, int max);
	void StoreString(const char** dest, const char* org);

#endif
