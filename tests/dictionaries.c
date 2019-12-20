/*-----------------------------

 [dictionaries.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

#include "japan-dictionary.h"

#ifdef JA_DEBUG
	#include <stdio.h>
	#define JA_DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
	#define JA_DEBUG_PRINT(...)
#endif


/*-----------------------------

 DictionaryTest1_FNV1Hash()
-----------------------------*/
extern void DictionaryTest1_FNV1Hash(void** cmocka_state)
{
	// Compared against https://sha256calc.com/hash/fnv1a64/
	(void)cmocka_state;

	uint64_t hash = jaFNV1Hash("Secuencia", strlen("Secuencia"));
	printf("'%s' = 0x%lX\n", "Secuencia", hash);
	assert_true((hash == 0x497b86722850e425));

	hash = jaFNV1Hash("Ruta", strlen("Ruta"));
	printf("'%s' = 0x%lX\n", "Ruta", hash);
	assert_true((hash == 0xea8ff32b70d99127));

	hash = jaFNV1Hash("Remolinos", strlen("Remolinos"));
	printf("'%s' = 0x%lX\n", "Remolinos", hash);
	assert_true((hash == 0xfb2a0331422c4485));
}


/*-----------------------------

 DictionaryTest2_SimpleUsage()
-----------------------------*/
static void inline CheckItem(const struct jaDictionaryItem* a, const struct jaDictionaryItem* b, int item, bool* error)
{
	if (a != b)
	{
		printf("Houston, we have a problems... '%s' != '%s' (item %i)\n", a->key, b->key, item);
		*error = true;
	}
}

static void IterationCallback(struct jaDictionaryItem* item, void* user_data)
{
	int* number = user_data;
	*number += 1;

	// Check if key matches with the content
	if (strcmp(item->key, "Primavera") == 0)
		assert_string_equal(item->data, "Primavera 0");
	else if (strcmp(item->key, "Sahumerio") == 0)
		assert_string_equal(item->data, "Sweet Sahumerio");
	else if (strcmp(item->key, "Ella") == 0)
		assert_string_equal(item->data, "Ella Uso Mi Cabeza Como Un Revolver");
	else if (strcmp(item->key, "Tormenta") == 0)
		assert_string_equal(item->data, "Ojo De La Tormenta");
	else if (strcmp(item->key, "Angel") == 0)
		assert_string_equal(item->data, "Angel Electrico");
	else if (strcmp(item->key, "X-Playo") == 0)
		assert_string_equal(item->data, "X-Playo");
	else if (strcmp(item->key, "Moire") == 0)
		assert_string_equal(item->data, "Moire");
	else
	{
		printf("Houston, seems like '%s' didn't get removed correctly\n", item->key);
		assert_false(true);
	}
}

extern void DictionaryTest2_SimpleUsage(void** cmocka_state)
{
	(void)cmocka_state;

	// Create a dictionary, we need a large
	// one so the rehashing mechanism get used
	struct jaDictionary* d = jaDictionaryCreate(NULL);

	struct jaDictionaryItem* secuencia = jaDictionaryAdd(d, "Secuencia", "Secuencia Inicial", 0);
	struct jaDictionaryItem* ruta = jaDictionaryAdd(d, "Ruta", "Toma La Ruta", 0);
	struct jaDictionaryItem* remolinos = jaDictionaryAdd(d, "Remolinos", "En Remolinos", 0);

	struct jaDictionaryItem* primavera = jaDictionaryAdd(d, "Primavera", "Primavera 0", 0);
	struct jaDictionaryItem* camaleon = jaDictionaryAdd(d, "Camaleon", "Camaleon", 0);
	struct jaDictionaryItem* luna = jaDictionaryAdd(d, "Luna", "Luna Roja", 0);
	struct jaDictionaryItem* sahumerio = jaDictionaryAdd(d, "Sahumerio", "Sweet Sahumerio", 0);
	struct jaDictionaryItem* ameba = jaDictionaryAdd(d, "Ameba", "Ameba", 0);

	struct jaDictionaryItem* fe = jaDictionaryAdd(d, "Fe", "Nuestra Fe", 0);
	struct jaDictionaryItem* claroscuro = jaDictionaryAdd(d, "Claroscuro", "Claroscuro", 0);
	struct jaDictionaryItem* fue = jaDictionaryAdd(d, "Fue", "Fue", 0);
	struct jaDictionaryItem* texturas = jaDictionaryAdd(d, "Texturas", "Texturas", 0);
	struct jaDictionaryItem* ella = jaDictionaryAdd(d, "Ella", "Ella Uso Mi Cabeza Como Un Revolver", 0);
	struct jaDictionaryItem* disco = jaDictionaryAdd(d, "Disco", "Disco Eterno", 0);
	struct jaDictionaryItem* zoom = jaDictionaryAdd(d, "Zoom", "Zoom", 0);
	struct jaDictionaryItem* tormenta = jaDictionaryAdd(d, "Tormenta", "Ojo De La Tormenta", 0);
	struct jaDictionaryItem* doppler = jaDictionaryAdd(d, "Doppler", "Efecto Doppler", 0);

	struct jaDictionaryItem* pasos = jaDictionaryAdd(d, "Pasos", "Pasos", 0);
	struct jaDictionaryItem* angel = jaDictionaryAdd(d, "Angel", "Angel Electrico", 0);
	struct jaDictionaryItem* crema = jaDictionaryAdd(d, "Crema", "Crema De Estrellas", 0);
	struct jaDictionaryItem* planta = jaDictionaryAdd(d, "Planta", "Planta", 0);
	struct jaDictionaryItem* xplayo = jaDictionaryAdd(d, "X-Playo", "X-Playo", 0);
	struct jaDictionaryItem* moire = jaDictionaryAdd(d, "Moire", "Moire", 0);

	// And because rehash means move items from
	// one bucket to another, lets retrieve them
	// checking if is the correct one
	bool e = false;

	CheckItem(jaDictionaryGet(d, "Moire"), moire, 1, &e);
	CheckItem(jaDictionaryGet(d, "X-Playo"), xplayo, 2, &e);
	CheckItem(jaDictionaryGet(d, "Planta"), planta, 3, &e);
	CheckItem(jaDictionaryGet(d, "Crema"), crema, 4, &e);
	CheckItem(jaDictionaryGet(d, "Angel"), angel, 5, &e);
	CheckItem(jaDictionaryGet(d, "Pasos"), pasos, 6, &e);
	CheckItem(jaDictionaryGet(d, "Doppler"), doppler, 7, &e);
	CheckItem(jaDictionaryGet(d, "Tormenta"), tormenta, 8, &e);
	CheckItem(jaDictionaryGet(d, "Zoom"), zoom, 9, &e);
	CheckItem(jaDictionaryGet(d, "Disco"), disco, 10, &e);
	CheckItem(jaDictionaryGet(d, "Ella"), ella, 11, &e);
	CheckItem(jaDictionaryGet(d, "Texturas"), texturas, 12, &e);
	CheckItem(jaDictionaryGet(d, "Fue"), fue, 13, &e);
	CheckItem(jaDictionaryGet(d, "Claroscuro"), claroscuro, 14, &e);
	CheckItem(jaDictionaryGet(d, "Fe"), fe, 15, &e);
	CheckItem(jaDictionaryGet(d, "Ameba"), ameba, 16, &e);
	CheckItem(jaDictionaryGet(d, "Sahumerio"), sahumerio, 17, &e);
	CheckItem(jaDictionaryGet(d, "Luna"), luna, 18, &e);
	CheckItem(jaDictionaryGet(d, "Camaleon"), camaleon, 19, &e);
	CheckItem(jaDictionaryGet(d, "Primavera"), primavera, 20, &e);
	CheckItem(jaDictionaryGet(d, "Remolinos"), remolinos, 21, &e);
	CheckItem(jaDictionaryGet(d, "Ruta"), ruta, 22, &e);
	CheckItem(jaDictionaryGet(d, "Secuencia"), secuencia, 23, &e);

	assert_false(e);

	// Remove some items in a random order
	JA_DEBUG_PRINT("\n####\n\n");

	jaDictionaryRemove(fe);
	jaDictionaryRemove(pasos);
	jaDictionaryRemove(secuencia);
	jaDictionaryRemove(remolinos);
	jaDictionaryRemove(texturas);
	jaDictionaryRemove(fue);
	jaDictionaryRemove(ruta);
	jaDictionaryRemove(camaleon);
	jaDictionaryRemove(zoom);
	jaDictionaryRemove(luna);
	jaDictionaryRemove(claroscuro);
	jaDictionaryRemove(crema);
	jaDictionaryRemove(ameba);
	jaDictionaryRemove(doppler);
	jaDictionaryRemove(disco);
	jaDictionaryRemove(planta);

	// Count if there are 7 items
	int number = 0;

	jaDictionaryIterate(d, IterationCallback, &number);
	assert_int_equal(number, 7);

	// Bye!
	jaDictionaryDelete(d);
}
