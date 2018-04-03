#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

struct user {
	char name[16];
	int age;
	bool is_hacker;
} __attribute__((packed));

int main() 
{
	struct user hacker;
	hacker.is_hacker = 0;
	hacker.age = 25;

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Hey, hacker! What's your name?\n");

	read(STDIN_FILENO, hacker.name, 30);

	if (hacker.age == 25 && hacker.is_hacker == 1) {
		printf("You made it! Hackers dont have an age limit!\n");
		system("/bin/sh");
	} else {
		printf("Oh noes, you're not old enough to be a hacker!\n");
	}

	return 0;
}
