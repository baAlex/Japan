Japan
=====
[![Ubuntu CI](https://github.com/baAlex/Japan/workflows/UbuntuCI/badge.svg)](https://github.com/baAlex/Japan/actions)
[![Windows CI](https://github.com/baAlex/Japan/workflows/WindowsCI/badge.svg)](https://github.com/baAlex/Japan/actions)

Personal C library.


Compilation
-----------
There are no runtime dependencies except for the standard library. Compilation dependencies only include *Ninja*, on Ubuntu you can install it with:
```
sudo apt install ninja-build
```

To compile:
```
cd Japan
ninja -f posix-release.ninja
```

Optionally you can compile a debug build with:
```
ninja -f posix-debug.ninja
```


License
-------
Under [MIT License](LICENSE).

### Optional additional notes
- File 'matrix.c' is licensed under the WTFPL license. Copyright (c) 2013 by Wolfgang 'datenwolf' Draxinger. Based on the library [linmath.h](https://github.com/datenwolf/linmath.h).
- File 'laws.c' is released under public domain. Based on code written by Steve Underwood (steveu@coppice.org): *g711.h - In line A-law and u-law conversion routines*.

As these terms can be problematic[[1]](https://en.wikipedia.org/wiki/WTFPL#Discussion)[[2]](https://en.wikipedia.org/wiki/Public_domain#Dedicating_works_to_the_public_domain), if desired it is safe to assume that listed files are re-licensed under the MIT License as specified in the file [LICENSE](LICENSE). Is not a requirement expose the previous list or this paragraph, here was done as a gratitude gesture.
