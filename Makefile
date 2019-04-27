
.SUFFIXES: .c .o .dbg.o

CC=cc
AR=ar rcs
RM=rm -f
CLOC=cloc
TIDY=clang-tidy

OUT=japan.a
OUT_DBG=japan-dbg.a

CFLAGS=-c -O3 -flto -mtune=generic -I./include
CFLAGS_DBG=-c -std=c11 -O0 -Wall -Wextra -pedantic -g -DDEBUG -I./include

TIDY_FLAGS=-checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-*
TIDY_DEFINITIONS=$(CFLAGS_DBG)

FILES=./source/buffer.c \
      ./source/endianness.c \
      ./source/error.c \
      ./source/list.c \
      ./source/sound.c \
      ./source/sound-au.c \
      ./source/tree.c \
      ./source/vector.c

default: debug
all: release debug

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.c.dbg.o:
	$(CC) $(CFLAGS_DBG) $< -o $@

release: $(FILES:.c=.o)
	$(AR) $(OUT) $(FILES:.c=.o)

debug: $(FILES:.c=.dbg.o)
	$(AR) $(OUT_DBG) $(FILES:.c=.dbg.o)

clean:
	$(RM) $(OUT)
	$(RM) $(OUT_DBG)
	$(RM) $(FILES:.c=.o)
	$(RM) $(FILES:.c=.dbg.o)

stats:
	$(CLOC) $(FILES)

tidy:
	$(TIDY) $(TIDY_FLAGS) $(FILES) -- $(TIDY_DEFINITIONS)
