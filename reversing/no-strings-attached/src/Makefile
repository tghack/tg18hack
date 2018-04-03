CC = gcc
CFLAGS = -Wall -Wextra -O0

.PHONY: all, run, clean
all: checker

checker: checker.c
	$(CC) $(CFLAGS) $^ -o $@

run: checker
	./$^

clean:
	rm -vf checker
