CC=gcc
CFLAGS=-Wall -Wextra -m32

.PHONY: all clean

all: pwn101

pwn101: pwn101.c
	$(CC) $(CFLAGS) $^ -o $@
	#mv $@ $@_`sha1sum $@`

clean:
	rm -vf pwn101
