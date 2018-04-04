CC=gcc
CFLAGS=-Wall -Wextra
CFLAGS+=-D_FORTIFY_SOURCE=2
CFLAGS+=-fpic -pie
CFLAGS+=-O2
CFLAGS+=-z relro -z now
CFLAGS+=-fstack-protector-strong

.PHONY: all run clean

all: defs

defs: defs.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -vf defs
