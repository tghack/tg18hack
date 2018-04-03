#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *fp;
	char buf[64] = { 0 };

	fp = fopen("/home/flag/flag.txt", "r");
	if (!fp) {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}

	if (!fgets(buf, sizeof(buf), fp)) {
		perror("fgets()");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	printf("%s", buf);

	fclose(fp);
}
