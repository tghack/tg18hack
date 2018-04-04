#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

static void print_menu(void)
{
	puts("1. read");
	puts("2. write");
	puts("3. quit");
	printf("> ");
}

static void do_read(void)
{
	uint64_t val, addr;
	char input[64] = { 0 };

	printf("address? ");
	if (!fgets(input, sizeof(input), stdin)) {
		perror("fgets()");
		exit(EXIT_FAILURE);
	}

	addr = strtoull(input, NULL, 16);
#if 0
	printf("0x%lx\n", addr);
#endif

	val = *(uint64_t *)addr;
	printf("value: 0x%lx\n", val);
}

static void do_write(void)
{
	uint64_t val, addr;
	char input[64] = { 0 };

	printf("address? ");
	if (!fgets(input, sizeof(input), stdin)) {
		perror("fgets()");
		exit(EXIT_FAILURE);
	}

	addr = strtoull(input, NULL, 16);

	printf("value? ");
	if (!fgets(input, sizeof(input), stdin)) {
		perror("fgets()");
		exit(EXIT_FAILURE);
	}

	val = strtoull(input, NULL, 16);

	printf("0x%lx = 0x%lx\n", addr, val);
	*(uint64_t *)addr = val;
}

int main(void)
{
	int num;
	char input[64] = { 0 };
	char name[64] = { 0 };

#if 1
	alarm(30);
#endif
	setvbuf(stdout, NULL, _IONBF, 0);

	printf("What's your name? ");
	if (!fgets(name, sizeof(name), stdin)) {
		perror("fgets()");
		exit(EXIT_FAILURE);
	}

	printf("Welcome ");
	printf(name);

	for (;;) {
		print_menu();
		if (!fgets(input, sizeof(input), stdin)) {
			perror("fgets()");
			exit(EXIT_FAILURE);
		}

		num = atoi(input);
		if (num == 1)
			do_read();
		else if (num == 2)
			do_write();
		else if (num == 3)
			break;
		else
			printf("Invalid choice %d!\n", num);
	}
	
	return 0;
}
