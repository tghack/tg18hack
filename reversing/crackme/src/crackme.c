/*
 * Some resources used:
 * https://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html
 * https://stackoverflow.com/questions/3898435/labels-in-gcc-inline-assembly
 * http://beefchunk.com/documentation/lang/c/pre-defined-c/precomp.html
 */
#ifdef __MINGW32__
#include <windows.h>
#endif
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

/* check functions, auto-generated */
#include "check.h"

#define DEBUG

#ifdef DEBUG
#define pr_debug(fmt, ...) \
	do { \
		fprintf(stderr, "[+] " fmt, __VA_ARGS__); \
	} while (0)
#else
#define pr_debug(fmt, ...)
#endif

#ifdef YOLO
#define VMCALL __asm__ __volatile__ ("vmcall\n\t")
#else
#define VMCALL __asm__ __volatile__ ("nop\n\t")
#endif


/*
 * jump over some crap instructions
 * jmp instruction is 2 bytes
 */
#define JUMP_CRAP \
	asm volatile ("jmp . + 6\n\t"); \
	asm volatile (".byte 0xde\n\t"); \
	asm volatile (".byte 0xad\n\t"); \
	asm volatile (".byte 0xbe\n\t"); \
	asm volatile (".byte 0xef\n\t")

#define FUNC_YAY 0
#define FUNC_FAIL 1
#define NUM_FUNCS 2

static void *funcs[NUM_FUNCS] = { NULL };

static char *read_user_input(const char *prompt)
{
	char buf[128] = { 0 };
	JUMP_CRAP;

	printf("%s", prompt);

	if (!fgets(buf, sizeof(buf), stdin)) {
		perror("fgets()");
		return NULL;
	}

	return strdup(buf);
}

static int check_password(const char *pw)
{
	int fail = 0;
	JUMP_CRAP;

	if (strlen(pw) != FLAG_SIZE)
		goto *funcs[FUNC_FAIL];

	for (size_t i = 0; pw[i]; i++) {
		if (!check_funcs[i])
			goto *funcs[FUNC_FAIL];

#if 0
		if (!check_funcs[i](pw[i]))
			goto *funcs[FUNC_FAIL];
#else
		if (!check_funcs[i](pw[i]))
			fail = 1;
#endif
	}

	if (fail)
		goto *funcs[FUNC_FAIL];
	goto *funcs[FUNC_YAY];

	return 1;
}

int main(void)
{
	char *password;

	/* init jumptable */
	funcs[FUNC_YAY] = &&yay_label;
	funcs[FUNC_FAIL] = &&fail_label;

	password = read_user_input("Password: ");
	if (!password)
		goto fail_label;

	password[strcspn(password, "\n")] = '\0';

#if 0
	pr_debug("input: %s\n", password);
#endif

	VMCALL;
	JUMP_CRAP;

	check_password(password);

yay_label:
	printf("yay!\n");
	exit(EXIT_SUCCESS);

fail_label:
	printf("nope!\n");
	exit(EXIT_FAILURE);

	return 0;
}
