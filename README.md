
LibJapan
========
[![Ubuntu CI](https://github.com/baAlex/libjapan/workflows/Ubuntu/badge.svg)][1]
[![Windows CI](https://github.com/baAlex/libjapan/workflows/Windows/badge.svg)][2]
[![CodeQL scanning](https://github.com/baAlex/libjapan/workflows/CodeQL%20scanning/badge.svg)][3]

A collection of frequently used features that the C language lacks: lists, hashes, trees, maths functions (vectors, matrices), between an etcetera of other utilities. My contribution to the constellation of existing general-purpose libraries.

The project born as an exercise to learn the language, since then I've been updating/refactoring it according to my needs and experience. There are some flaws to fix, designs to polish, bugs to add, and probably the library is too game-development oriented, I'm working on those aspects... any help is welcomed.


Overview
--------
| Header            | -
| ----------------- | -
| [dictionary.h][4] | Linear hash map, grows dynamically between a threshold. By default uses FNV1 as hash function.
| [endianness.h][5] | Runtime functions to convert between little and big endianness.
| [image.h][6]      | Support for the unknown SGI format, up to 4 channels of 8 or 16 bits, optionally with RLE compression.
| [list.h][7]       | Double linked list. Friendly with the "fat pointer" technique.
| [matrix.h][8]     | 4x4 matrix operations.
| [sound.h][9]      | Support for WAV and AU formats, with integer-samples of 8, 16, 32 bits or float-samples of 32, 64 bits. Handles U-Law and A-Law compression.
| [status.h][10]    | A method to pass error values between functions.
| [string.h][11]    | UTF8 and ASCII validation. Distinguishes between UTF8 units and bytes, can be used to count them.
| [tree.h][12]      | Just a generic tree.
| [utilities.h][13] | Min, Max, DegToRad, and other one-liners.
| [vector.h][14]    | 2, 3, and 4 dimensions vectors.


### Incomplete modules:
| Header                | -
| --------------------- | -
| [token.h][15]         | A tokenizer, as C makes string manipulation particularly hard.
| [configuration.h][16] | Configuration variables (cvar) a la Quake.
| [buffer.h][17]        | I'm thinking on delete it...


Compilation
-----------
Compiles lovely with Clang, GCC and MSVC ❤️.

To clone and compile the repository with:
```
git clone https://github.com/baAlex/libjapan.git
cd libjapan

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

And for a release build the same procedure as above except for:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

By default if CMake finds `cmocka` installed, a test suite is build.


Similar projects
----------------
- [Apache Portable Runtime](https://apr.apache.org/)
- [GLib](https://developer.gnome.org/glib/stable/)

Modern, simpler approaches:
- [stb.h](https://github.com/nothings/stb/blob/master/stb.h), «This is a single header file with a bunch of useful utilities for getting stuff done in C/C++».
- [Tbox](https://github.com/tboox/tbox), «The project focuses on making C development easier and provides many modules (...), so that any developer can quickly pick it up and enjoy the productivity boost when developing in C language».
- [ZPL](https://github.com/zpl-c/zpl), «C99 cross-platform header-only library that offers powerful toolkit to accelerate your development progress».


License
-------
Under [MIT License](LICENSE).

### Optional notes
- File 'matrix.c' is licensed under the WTFPL license. Copyright (c) 2013 by Wolfgang 'datenwolf' Draxinger. Based on the library [linmath.h][20].
- File 'laws.c' is released under public domain. Based on code written by Steve Underwood (steveu@coppice.org): *g711.h - In line A-law and u-law conversion routines*.

As these terms can be problematic[(a)][18][(b)][19], if desired it is safe to assume that listed files are re-licensed under the MIT License as specified in the file [LICENSE](LICENSE). Is not a requirement expose the previous list or this paragraph, here was done as a gratitude gesture.

____

[1]: https://github.com/baAlex/libjapan/actions?query=workflow%3AUbuntu
[2]: https://github.com/baAlex/libjapan/actions?query=workflow%3AWindows
[3]: https://github.com/baAlex/libjapan/security/code-scanning
[4]: ./include/japan-dictionary.h
[5]: ./include/japan-endianness.h
[6]: ./include/japan-image.h
[7]: ./include/japan-list.h
[8]: ./include/japan-matrix.h
[9]: ./include/japan-sound.h
[10]: ./include/japan-status.h
[11]: ./include/japan-string.h
[12]: ./include/japan-tree.h
[13]: ./include/japan-utilities.h
[14]: ./include/japan-vector.h
[15]: ./include/japan-token.h
[16]: ./include/japan-configuration.h
[17]: ./include/japan-buffer.h
[18]: https://en.wikipedia.org/wiki/WTFPL#Discussion
[19]: https://en.wikipedia.org/wiki/Public_domain#Dedicating_works_to_the_public_domain
[20]: https://github.com/datenwolf/linmath.h
