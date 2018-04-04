#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd;
	char buf[256] = { 0 };
	const char *pass = "_super__duper__secret__1337__key";
	//const char *pass = "_____________wyfudnywufdnwfdun__";

	fd = open("/dev/tghack", O_RDWR);
	if (fd == -1) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	strcpy(buf, pass);

	if (write(fd, buf, strlen(buf)) <= 0) {
		perror("write()");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("yay! flag: %s\n", buf);

	close(fd);
	return 0;
}
