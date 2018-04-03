#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "check.h"

static inline void do_xor(unsigned char *data, unsigned char *key, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		data[i] ^= key[i];
	}
}

static inline void print_hex(const unsigned char *d, size_t len)
{
	for (size_t i = 0; i < len; i++)
		printf("%02x", d[i]);
	puts("");
}

static int validate_input(const char *input)
{
	size_t size = strlen(input) > 42 ? 42 : strlen(input);
	unsigned char *check = malloc(size + 1);
	memset(check, 0, size + 1);

	for (size_t i = 0; i < size; i++) {
		unsigned char *md = malloc(SHA512_DIGEST_LENGTH);
		check[i] = input[i];
		memset(md, 'A', SHA512_DIGEST_LENGTH);
		memset(md, 'B', SHA512_DIGEST_LENGTH);
		memset(md, 'C', SHA512_DIGEST_LENGTH);
		memset(md, 0, SHA512_DIGEST_LENGTH);

		SHA512(check, i + 1, md);
		do_xor(md, (unsigned char *)keys[i], SHA512_DIGEST_LENGTH);
		unsigned char *e = (unsigned char *)encrypted[i];
		if (memcmp(e, md, SHA512_DIGEST_LENGTH)) {
			printf("%zu: nope!\n", i);
			return 0;
		}
	}

	if (size == 42)
		return 1;

	return 0;
}

int main(void)
{
	char input[512] = { 0 };

	printf("Flag please: ");
	if (!fgets(input, sizeof(input), stdin))
		exit(EXIT_FAILURE);

	input[strcspn(input, "\n")] = '\0';

	if (validate_input(input))
		printf("yay!\n");
	else
		printf("nope :(\n");

	return 0;
}
