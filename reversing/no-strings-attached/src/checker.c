#include <stdio.h>
#include <string.h>

static const char enc[] = "\x2e\x74\x43\x08\x24\x2a\x6f\x47\x58\x2c\x28\x58\x0a\x6e\x06\x54\x2d\x52\x18\x03\x07\x44\x00\x17\x58\x03\x33\x2c\x03\x46\x42\x31\x0f\x40\x0f";

static __attribute__((noinline)) void print_flag(const char *pw)
{
	size_t len = sizeof(enc) - 1;
	size_t pwlen = strlen(pw);
	char dec[sizeof(enc)] = { 0 };

	for (size_t i = 0; i < len; i++) {
		dec[i] = enc[i] ^ pw[i % pwlen];
	}

	printf("%s\n", dec);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <password>\n", argv[0]);
		return 0;
	}

	if (!strcmp(argv[1], "z3r0_c00l_w4s_h3r3")) {
		printf("Congrats! The flag is: ");
		print_flag(argv[1]);
	} else {
		printf("Nope :(\n");
	}

	return 0;
}
