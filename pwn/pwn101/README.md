# Pwn 101: Memory Corruption
**Points: 50**

**Author: maritio_o**

In this task, we present the vulnerability with **memory corruption**. Your task is to overwrite one of the variables. Unfortunately there is no input for this variable. You must analyze the source code and find out how to overwrite it. You may follow this list: 

* Read the source code, and find a variable that must be changed by memory corruption. You must use memory corruption because there is no other way to change it.
* To change something, such as a variables, return addresses or metadata, with memory corruption, it must be overwritten. Find a way to overwrite the variable, and the code will provide you a shell. (tips: Beware of the other variable!)
* Now that you have a shell with root access, find the flag! (tips: try `id` or `whoami` as a command in the shell if you think you got a shell)

To connect to our service, run the following command in your terminal:
```
nc pwn101.tghack.no 1060
```

> If you need any help, please don't hesitate with coming to the TGHack area in the Creative Lounge! We will help you with solving all the noob tasks.

[Here](./src/pwn101) is the executable file. You find the source code [here](./src/pwn101.c), or you may look at it below: 
```C
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

struct user {
	char name[16];
	int age;
	bool is_hacker;
} __attribute__((packed)); //evt no reordering

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
```

> In addition, the following tools will be useful to solve this task:

> * The program is written in C, which might be useful information for googling! You may also use the man pages to read about C functions by writing `$ man functionname` into the terminal.  
> * It might be useful to look into how the stack and the memory works, and maybe try to draw the stack on paper. [Read about it here!](https://en.wikipedia.org/wiki/Data_segment)
> * A debugger called [gdb-peda](https://github.com/longld/peda)
>	* Run with `gdb-peda$ r`
>	* Disassemble with `gdb-peda$ disas functionname`. In this task `gdb-peda$ disas main` is enough, as there are no functions.
>	* Set breakpoints using `b* memoryaddress`. If you have stopped at a breakpoint and want to continue, enter `gdb-peda$ c`. To delete breakpoints enter `gdb-peda$ d`.
> * For this task, it is useful to script a bit. I like Python, and use a package called [**pwn-tools**](https://github.com/Gallopsled/pwntools) to communicate with the server by sending and recieving data. If you think Python is scary, you should take a look at [this page](https://www.codecademy.com/learn/learn-python), and then I think you will change your mind! And remember that we are more than happy to help you at the TGHack area in the Creative Lounge!
