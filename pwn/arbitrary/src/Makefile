CC=gcc
CFLAGS=-Wall -Wextra
CFLAGS+=-z relro -z now
CFLAGS+=-fpic -pie
CFLAGS+=-D_FORTIFY_SOURCE=2 -O2
CFLAGS+=-Wno-format-security

.PHONY: all, run, clean
all: main

main: main.c
	$(CC) $(CFLAGS) $^ -o $@

run: main
	socat tcp-listen:1060,fork,reuseaddr exec:./$^

pwn: main
	python2 solve.py

clean:
	rm -vf main
