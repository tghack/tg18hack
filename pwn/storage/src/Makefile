CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99
CFLAGS+=-z relro -z now
CFLAGS+=-fstack-protector-strong
CFLAGS+=-D_FORTIFY_SOURCE=2
CFLAGS+=-fpic -pie
CFLAGS+=-O2
LFLAGS=-lpthread

.PHONY: all clean run
all: service

service: service.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

run: service
	socat tcp-listen:4444,fork,reuseaddr EXEC:./$^,stderr

check: service
	valgrind --leak-check=full ./$^

clean:
	rm -f service
