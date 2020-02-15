cmake_minimum_required(VERSION 3.10)

project("LibJapan" C)
include_directories("./include/")

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_C_COMPILER_ID MATCHES "GNU"))
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -DJA_EXPORT_SYMBOLS")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -std=c11 -Wall -Wextra -Wconversion -pedantic -DJA_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -mtune=generic")
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DJA_EXPORT_SYMBOLS")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi /DJA_DEBUG /D_CRT_SECURE_NO_WARNINGS")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /D_CRT_SECURE_NO_WARNINGS")
endif()

set(SOURCES
	"./source/aabounding.c"
	"./source/buffer.c"
	"./source/dictionary.c"
	"./source/endianness.c"
	"./source/list.c"
	"./source/matrix.c"
	"./source/status.c"
	"./source/tree.c"
	"./source/utilities.c"
	"./source/vector.c"
	"./source/configuration/arguments.c"
	"./source/configuration/configuration.c"
	"./source/configuration/file.c"
	"./source/image/format-sgi.c"
	"./source/image/image.c"
	"./source/sound/format-au.c"
	"./source/sound/format-wav.c"
	"./source/sound/laws.c"
	"./source/sound/sound.c")

add_library("japan-static" STATIC ${SOURCES})
add_library("japan" SHARED ${SOURCES})


# Test suite
find_package("cmocka")

if (cmocka_FOUND)

	set(TEST_SUITE_SOURCES
		"./tests/configurations.c"
		"./tests/dictionaries.c"
		"./tests/images.c"
		"./tests/lists.c"
		"./tests/test-suite.c")

	add_executable("test-suite" ${TEST_SUITE_SOURCES})
	target_link_libraries("test-suite" "japan-static")
	target_link_libraries("test-suite" "cmocka")
	target_link_libraries("test-suite" "m")

endif (cmocka_FOUND)
