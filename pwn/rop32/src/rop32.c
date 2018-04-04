/*
* ROP 32-bit
* Why Return Oriented Programming (ROP)?
* - NX (Non executable stack) was invented,
* so that it would not be possible to inject
* code, such as shellcode, straight into the 
* stack and execute it.
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

// To make task easier, by faking info-leak of memory addresses
static void __attribute__((constructor)) print_system(void)
{
	void *addr;

	addr = dlsym(RTLD_NEXT, "system");
	if (!addr)
		exit(EXIT_FAILURE);

	printf("system: %p\n", addr);
}

static void print_banner(void)
{
	FILE *fp;
	char *buf;
	size_t size;

	fp = fopen("banner.txt", "r");
	if (!fp) {
		perror("fopen(banner.txt)");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	buf = calloc(1, size);
	rewind(fp);

	/* -1 to drop newline */
	if (fread(buf, size - 1, 1, fp) < 1) {
		perror("fread()");
		exit(EXIT_FAILURE);
	}

	fclose(fp);

	printf("%s\n", buf);
}

static void function(void) 
{
	char buffer[48]; // Buffer size should be a multiple of 4. (delelig på 4) 

	print_banner();
	read(STDIN_FILENO, buffer, 72);
}

int main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	function();

	return 0;
}
