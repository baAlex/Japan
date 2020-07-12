
LibJapan
========
[![Ubuntu CI](https://github.com/baAlex/libjapan/workflows/Ubuntu/badge.svg)](https://github.com/baAlex/libjapan/actions?query=workflow%3AUbuntu)
[![Windows CI](https://github.com/baAlex/libjapan/workflows/Windows/badge.svg)](https://github.com/baAlex/libjapan/actions?query=workflow%3AWindows)
[![CodeQL scanning](https://github.com/baAlex/libjapan/workflows/CodeQL%20scanning/badge.svg)](https://github.com/baAlex/libjapan/security/code-scanning)

- *(2020/7/7) Windows CI fails as Cmocka dependency is broken in VCPKG[[1]](https://github.com/microsoft/vcpkg/pull/12196).*

A collection of frequently used features that the C language lacks: lists, hashes, trees, maths functions (vectors, matrices), between an etcetera of other utilities. Nothing different to existing general-purpose libraries.

The project born as an exercise to learn the language and since then I been updating/refactoring it according to my needs and experience. There are some flaws to fix, designs to polish, and probably the library is too game-development oriented, I'm working on those aspects... so any help or suggestion is more that welcomed.


Compilation
-----------
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
- File 'matrix.c' is licensed under the WTFPL license. Copyright (c) 2013 by Wolfgang 'datenwolf' Draxinger. Based on the library [linmath.h](https://github.com/datenwolf/linmath.h).
- File 'laws.c' is released under public domain. Based on code written by Steve Underwood (steveu@coppice.org): *g711.h - In line A-law and u-law conversion routines*.

As these terms can be problematic[[1]](https://en.wikipedia.org/wiki/WTFPL#Discussion)[[2]](https://en.wikipedia.org/wiki/Public_domain#Dedicating_works_to_the_public_domain), if desired it is safe to assume that listed files are re-licensed under the MIT License as specified in the file [LICENSE](LICENSE). Is not a requirement expose the previous list or this paragraph, here was done as a gratitude gesture.
