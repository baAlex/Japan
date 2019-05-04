
.SUFFIXES: .c .o .dbg.o

CC=cc
LD=ld
AR=ar
RM=rm -f

OUT_STATIC=japan.a
OUT_SHARED=libjapan.so
OUT_STATIC_DBG=japan-dbg.a
OUT_SHARED_DBG=libjapan-dbg.so

CFLAGS=-c -O3 -fpic -mtune=generic -fvisibility=hidden -I./include
CFLAGS_DBG=-c -O0 -fpic -std=c11 -Wall -Wextra -pedantic -g -I./include -DDEBUG

LFLAGS_STATIC=rcs
LFLAGS_SHARED=-lm -shared

FILES=./source/buffer.c \
      ./source/endianness.c \
      ./source/error.c \
      ./source/image.c \
      ./source/image-sgi.c \
      ./source/list.c \
      ./source/sound.c \
      ./source/sound-au.c \
      ./source/sound-wav.c \
      ./source/tree.c \
      ./source/vector.c

default: debug
all: release debug

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.c.dbg.o:
	$(CC) $(CFLAGS_DBG) $< -o $@

release: $(FILES:.c=.o)
	$(AR) $(LFLAGS_STATIC) $(OUT_STATIC) $(FILES:.c=.o)
	$(LD) $(FILES:.c=.o) $(LFLAGS_SHARED) -o $(OUT_SHARED)

debug: $(FILES:.c=.dbg.o)
	$(AR) $(LFLAGS_STATIC) $(OUT_STATIC_DBG) $(FILES:.c=.dbg.o)
	$(LD) $(FILES:.c=.dbg.o) $(LFLAGS_SHARED) -o $(OUT_SHARED_DBG)

clean:
	$(RM) $(OUT_STATIC)
	$(RM) $(OUT_STATIC_DBG)
	$(RM) $(OUT_SHARED)
	$(RM) $(OUT_SHARED_DBG)
	$(RM) $(FILES:.c=.o)
	$(RM) $(FILES:.c=.dbg.o)

####

stats:
	cloc $(FILES)

symbols: release
	nm -D $(OUT_SHARED)

tidy:
	clang-tidy -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* $(FILES) -- $(CFLAGS_DBG)
