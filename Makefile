
FILES=./source/buffer.c \
      ./source/dictionary.c \
      ./source/endianness.c \
      ./source/image.c \
      ./source/image-sgi.c \
      ./source/list.c \
      ./source/matrix.c \
      ./source/sound.c \
      ./source/sound-au.c \
      ./source/sound-wav.c \
      ./source/status.c \
      ./source/tree.c \
      ./source/vector.c

default:
	@echo "Use 'ninja' to build the library\n"
	@echo "This makefile is meant to launch:"
	@echo " - make stats        To display the lines of code"
	@echo " - make symbols      To display exported symbols"
	@echo " - make tidy         To run clang-tidy"

stats:
	cloc $(FILES)

symbols:
	nm -D libjapan.so

tidy:
	clang-tidy -checks=clang-analyzer-*,bugprone-*,cert-*,performance-*,portability-* $(FILES) -- -I./include
