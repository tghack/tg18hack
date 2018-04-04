#CC=gcc -m32
CC=clang -m32 # noen ganger putter gcc inn shit på stacken
CFLAGS=-Wall -Wextra
CFLAGS+=-fno-stack-protector # skru av stack cookies
CFLAGS+=-z execstack # skru av NX
CFLAGS+=-fno-pie # ikke randomiser addressen til binærfilen
CFLAGS+=-Wl,-z,norelro # skru av RELRO (GOT er skrivbar)

.PHONY: all, run, clean
all: pwn102

flip: pwn102.c
	$(CC) $(CFLAGS) $^ -o $@

run: pwn102
	./$^

clean:
	rm -vf pwn102
