/*
 * simple shellcode "host" that whitelists required syscalls
 *
 * clock_gettime
 * sleep
 * mmap
 * madvise
 * munmap
 * socket
 * connect
 * write
 * read
 * close
 * exit
 * group_exit
 */
#include <stdio.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/mman.h>
#include <string.h>

/* from linux/seccomp.h */
#define SECCOMP_MODE_FILTER 2

/* https://www.kernel.org/doc/Documentation/networking/filter.txt */
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

struct sock_filter {	/* Filter block */
	__u16	code;   /* Actual filter code */
	__u8	jt;	/* Jump true */
	__u8	jf;	/* Jump false */
	__u32	k;      /* Generic multiuse field */
};

struct sock_fprog {		/* Required for SO_ATTACH_FILTER. */
	unsigned short	len;	/* Number of filter blocks */
	struct sock_filter *filter;
};

/* auto-generated, contains the actual bpf filter */
#include "filter.h"

struct sock_fprog filterprog = {
	.len = sizeof(filter) / sizeof(filter[0]),
	.filter = filter
};

static void __attribute__((noreturn)) do_exit(const char *msg)
{
	ssize_t ret;

	ret = write(STDERR_FILENO, msg, strlen(msg));
	(void)ret;
	exit(EXIT_FAILURE);
}

int main(void)
{
	char *code;

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
		do_exit("prctl(PR_SET_NO_NEW_PRIVS, 1)");

	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &filterprog))
		do_exit("prctl(PR_SET_SECCOMP, ...)");

	code = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (code == MAP_FAILED)
		do_exit("mmap error :(");

	if (read(STDIN_FILENO, code, 4096) <= 0)
		do_exit("read error :(");

	((void (*)(void))code)();

	exit(EXIT_SUCCESS);
}
