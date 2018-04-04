# Pwn 102: Buffer Overflow
**Points: 100**

**Author: maritio_o**

Now that you've learned a little more about how the stack works (assuming you finished pwn 101), I want to test your new skills and maybe teach you a little more about pwning, more specific a vulnerability called **buffer overflow**! 
As another hint, I will tell you this type buffer overflow is called **stack overflow**.

In this task, you should read and analyze the source code to find the vulnerability. 
When you think you know the vulnerability in the code, you should use a debugger to get knowledge of the stack in order to manipulate it to paste the flag. 

```
nc pwn102.tghack.no 1062
```

> If you need any help, please don't hesitate with coming to the TGHack area in the Creative Lounge! We will help you with solving all the noob tasks.

The binary is [here](src/pwn102), and you can find the source code [here](src/pwn102.c), or look at the snippet below.

```C
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void supersecret(void)
{
	system("cat flag.txt");
}

int main(void) 
{
	char name[20];

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Welcome hackerman!\nPlease enter your secret password to go back in time: ");

	read(STDIN_FILENO, name, 64);

	return 0;
}

```

> To help you with this introductory pwn task, I provide you this list of suggestions:

> * Notice that the source code expects a file called **flag.txt**. When debugging locally, you should make a temporary file  with the same name.
> * This file is a 32-bit binary. Go to the ["How to run binaries" section](https://github.com/PewZ/tg18hack/blob/master/other/webpage/tips_n_tricks.md) in the Tips 'n' Tricks-page to check out how to run it.
> * **gdb-peda** is useful for this task. Read about hwo to use the tool in the [gdb-peda section](https://github.com/PewZ/tg18hack/blob/master/other/webpage/tips_n_tricks.md) in the Tips 'n' Tricks-page.
