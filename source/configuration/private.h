/*-----------------------------

 [configuration/private.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JA_CONFIG_PRIVATE_H
#define JA_CONFIG_PRIVATE_H

	#include <ctype.h>
	#include <limits.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <string.h>

	#define JA_WIP
	#include "japan-configuration.h"

	#include "japan-buffer.h"
	#include "japan-dictionary.h"
	#include "japan-utilities.h"

	enum Type
	{
		TYPE_INT = 0,
		TYPE_FLOAT,
		TYPE_STRING
	};

	enum SetBy
	{
		SET_DEFAULT = 0,
		SET_BY_ARGUMENTS,
		SET_BY_FILE
	};

	union Value
	{
		int i;
		float f;
		struct jaBuffer s;
	};

	struct jaCvar
	{
		struct jaDictionaryItem* item;

		enum Type type;
		enum SetBy set_by;

		union Value min;
		union Value max;
		union Value value;
	};

	enum jaStatusCode Store(struct jaCvar* cvar, const char* token, enum SetBy);

#endif
