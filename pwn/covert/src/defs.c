#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(void)
{
	printf("CLOCK_MONOTONIC_RAW: %d\n", CLOCK_MONOTONIC_RAW);
	printf("PROT_READ | PROT_WRITE: %d\n", PROT_READ | PROT_WRITE);
	printf("MAP_ANONYMOUS | MAP_PRIVATE: %d\n", MAP_ANONYMOUS | MAP_PRIVATE);
	printf("MADV_MERGEABLE: %d\n", MADV_MERGEABLE);
	printf("AF_INET: %d\n", AF_INET);
	printf("SOCK_STREAM: %d\n", SOCK_STREAM);
	printf("IPPROTO_TCP: %d\n", IPPROTO_TCP);
	printf("htons(4040): 0x%04x\n", htons(4040));

	return 0;
}
