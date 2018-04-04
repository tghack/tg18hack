CC=gcc
CFLAGS=-Wall -Wextra -O3 -static
LFLAGS=-lcrypto

.PHONY: all clean

all: flagcheck

flagcheck: flagcheck.c
	python2 gen.py
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)
	strip $@
	mv flagcheck flagcheck_`sha1sum flagcheck | awk '{print $$1}'`

solve: flagcheck
	python2 solve.py `ls | grep flagcheck_ | head -1`

clean:
	rm -vf flagcheck_*
