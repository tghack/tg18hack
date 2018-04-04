CC=musl-gcc
CFLAGS=-Wall -Wextra
CFLAGS+=-D_FORTIFY_SOURCE=2
CFLAGS+=-fpic -pie
CFLAGS+=-O2
CFLAGS+=-z relro -z now
CFLAGS+=-fstack-protector-strong

.PHONY: all run clean

all: server

server: server.c
	$(CC) $(CFLAGS) $^ -o run/$@
	cp flag.txt run/

run: server
	./$^ 127.0.0.1 4040

clean:
	rm -vf run/server run/flag.txt
