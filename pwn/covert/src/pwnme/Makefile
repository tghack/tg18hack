CC=musl-gcc
CFLAGS=-Wall -Wextra
CFLAGS+=-D_FORTIFY_SOURCE=2
CFLAGS+=-fPIC -pie
CFLAGS+=-O2
CFLAGS+=-z relro -z now
CFLAGS+=-fstack-protector-strong

.PHONY: all run clean

all: pwnme

pwnme: pwnme.c
	echo "struct sock_filter filter[] = {" > filter.h
	bpfc filter >> filter.h
	echo "};" >> filter.h
	$(CC) $(CFLAGS) $^ -o run/$@

clean:
	rm -vf run/pwnme filter.h
