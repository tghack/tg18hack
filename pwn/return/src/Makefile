CC=gcc
#CC=clang # noen ganger putter gcc inn shit på stacken
CFLAGS=-Wall -Wextra -m32
CFLAGS+=-fno-stack-protector # skru av stack cookies
CFLAGS+=-no-pie # -fno-pie for clang (ikke randomiser addressen til binærfilen)

.PHONY: all, run, clean
all: rop32

rop32: rop32.c
	$(CC) $(CFLAGS) $^ -o $@ -ldl

run: rop32
	./$^

clean:
	rm -vf rop32
