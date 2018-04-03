CC=i686-w64-mingw32-gcc
STRIP=i686-w64-mingw32-strip
CFLAGS=-Wall -Wextra -O0 -std=gnu99

.PHONY: all clean
all: crackme.exe

crackme.exe: crackme.c
	python2 gen_check_funcs.py > check.h
	$(CC) $(CFLAGS) $^ -o $@
	$(STRIP) -s $@
	gcc $(CFLAGS) $^ -o crackme
	strip -s crackme

clean:
	rm -vf crackme.exe crackme check.h
