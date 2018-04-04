# Flagcheck writeup

First of all, let's see what we're dealing with!


```
$ file flagcheck_a3419e0cdf73504421b4aaf5039865f22bb4436a
flagcheck_a3419e0cdf73504421b4aaf5039865f22bb4436a: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.32, BuildID[sha1]=c4bfe445d45f3a807ccc71a7a9edf55497a4442c, stripped
$ du -c flagcheck_a3419e0cdf73504421b4aaf5039865f22bb4436a
1.0M	flagcheck_a3419e0cdf73504421b4aaf5039865f22bb4436a
```

Pretty huge binary! Since it's a static binary, we can't use [`ltrace`](http://man7.org/linux/man-pages/man1/ltrace.1.html) because it relies on hooking dynamic library functions. Opening the binary in IDA or some disassembler, we can see that the main function performs _a lot_ of weird operations on our input.

By glancing over the disassembly, we can see that the input length is capped at 42, and that the program seems to check one byte at a time. One way to solve binaries like this is to count the number of instructions executed instead of spending a lot of time trying to understand what the binary does.

We'll accomplish this by using Pin! This is a binary instrumentation tool from Intel that has a lot of cool examples, including one to count the number of instructions executed by a program.

Download Pin from [here](https://software.intel.com/en-us/articles/pin-a-binary-instrumentation-tool-downloads).

Build the instruction counting tool:
```
$ tar xvf pin-3.5-97503-gac534ca30-gcc-linux.tar.gz
$ cd pin-3.5-97503-gac534ca30-gcc-linux/source/tools/ManualExamples
$ make -j4
```

The idea of our solution script is to first count the number of instructions to check the first part of the flag we know is correct: `TG18{`.

Then we'll keep checking characters until the instruction count increases, which means we've found a new character of the flag.

See [the solution script](src/solve.py) for the full implementation.

```
$ python2 solve.py ../flagcheck_a3419e0cdf73504421b4aaf5039865f22bb4436a
	[...]
TG18{g0_g0_g0_w3_h4v3_t0_ch3ck_m0r3_fl4gs}
```
