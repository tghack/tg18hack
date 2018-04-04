# Real Mode Writeup

Connecting to the server, we get the following prompt:
```
Give me data (512 bytes):
```

Upon sending it 512 bytes of A's, we can see that the server is trying to start up a VM!
```
SeaBIOS (version rel-1.11.0-5-g14d91c3-dirty-20180124_095838-zero-cool)
The flag is at 0x8000, can you read it?
Booting from Hard Disk...
Boot failed: not a bootable disk

Booting from Floppy...
Boot failed: could not read the boot disk

Booting from DVD/CD...
Boot failed: Could not read from CDROM (code 0003)
No bootable device.
```

We can also see that the flag is stored in memory at address 0x8000!

One way of solving this task is to write a tiny bootloader that is capable of printing characters to the screen. The solution can be found [here](src/loader.s).

The code is commented and should be OK to follow. In pseudo-C, the code would look like this:
```C
char *ptr = 0x8000;
while (*ptr != '\0') {
	putchar(*ptr);
	ptr++;
}
```

We use a BIOS interrupt to print characters to the screen.


We can put this in a script that will send the compiled bootloader to the server and read the flag back like this:
```python
from pwn import *

r = remote("real.tghack.no", 8086)

r.recvuntil(": ")
loader = open("bin/loader.img", "rb").read()
assert(len(loader) == 512)
r.send(loader)

#print r.recvuntil("SeaBIOS")[-7:]
r.recvline()
r.interactive()
```

```
$ python2 solve.py
[+] Opening connection to real.tghack.no on port 8086: Done
[*] Switching to interactive mode
		[ ... ]
The flag is at 0x00008000, can you read it?
Booting from Hard Disk...
TG18{16_bit_asm_warrior}
```

Tada!
