
cmake_minimum_required(VERSION 3.12)
project("LibJapan" C)

option(JAPAN_EXPORT_SYMBOLS "Export symbols" ON)
option(JAPAN_SHARED "Build shared library"   ON)
option(JAPAN_STATIC "Build static library"   ON)
option(JAPAN_BUILD_TEST "Build test suite"   ON)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

if (JAPAN_EXPORT_SYMBOLS)
	add_compile_definitions(JA_EXPORT_SYMBOLS)
endif ()

set(JAPAN_SOURCES
	"./source/buffer.c"
	"./source/dictionary.c"
	"./source/endianness.c"
	"./source/list.c"
	"./source/matrix.c"
	"./source/status.c"
	"./source/string.c"
	"./source/tree.c"
	"./source/utilities.c"
	"./source/vector.c"
	"./source/version.c"
	"./source/configuration/arguments.c"
	"./source/configuration/configuration.c"
	"./source/configuration/file.c"
	"./source/image/format-sgi.c"
	"./source/image/image.c"
	"./source/sound/format-au.c"
	"./source/sound/format-wav.c"
	"./source/sound/laws.c"
	"./source/sound/sound.c"
	"./source/token/encode-ascii.c"
	"./source/token/encode-utf8.c"
	"./source/token/token.c")

if (JAPAN_SHARED)
	add_library("japan" SHARED ${JAPAN_SOURCES})
	target_include_directories("japan" PUBLIC "./include/")

	if (NOT MSVC)
		target_link_libraries("japan" PRIVATE "m")
	endif ()
endif ()

if (JAPAN_STATIC)
	add_library("japan-static" STATIC ${JAPAN_SOURCES})
	target_include_directories("japan-static" PUBLIC "./include/")

	if (NOT MSVC)
		target_link_libraries("japan-static" PRIVATE "m")
	endif ()
endif ()

if (JAPAN_BUILD_TEST)
	find_package(cmocka CONFIG)
	if (cmocka_FOUND)

		add_executable("test-suite"
			"./tests/configurations.c"
			"./tests/dictionaries.c"
			"./tests/images.c"
			"./tests/lists.c"
			"./tests/strings.c"
			"./tests/tokens.c"
			"./tests/test-suite.c")

		target_link_libraries("test-suite" PRIVATE "japan-static")
		target_include_directories("test-suite" PRIVATE ${CMOCKA_INCLUDE_DIR})

		if (DEFINED VCPKG_TOOLCHAIN)
			target_link_libraries("test-suite" PRIVATE cmocka::cmocka)
		else()
			target_link_libraries("test-suite" PRIVATE ${CMOCKA_LIBRARIES})
		endif()

	endif (cmocka_FOUND)
endif (JAPAN_BUILD_TEST)
