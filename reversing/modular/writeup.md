# Modular Writeup

The first clue here is the file extension: `.ko`. This tells us that this is a Linux kernel module. Also, the hint in the task description tells us the Linux kernel version we need to use: `4.4.0-109-generic`. TODO: change this later. we need to recompile right before TG to support the newest kernel version on Ubuntu 16.04.
If testing this challenge after the competition has ended, you might need to build the kernel module yourself. Install the linux kernel headers for your distribution (Ubuntu: `sudo apt install -y linux-headers-$(uname -r)`) and run `make` inside the `src` directory.

Because it's a hassle to debug Linux kernel modules, we start with static analysis. Most reversing tools should work fine with kernel modules, for example IDA or radare2.

Since kernel modules are ELF files, we can use `readelf` to get some basic information about the module. For example, we can see some interesting function names:
* `tg_flush`
* `test_skcipher_cb`
* `tg_read`
* `tg_write`
* `tg_init`
* `tg_exit`

By running `readelf --syms`, we can see the functions used by the module as well. Some of the more interesting ones are `misc_register()` and `misc_deregister()`. These functions tell us that we're dealing with a misc device driver. Misc drivers are made for implementing custom stuff that doesn't fit anywhere else in the kernel.

Let's take a look at the `misc_register()` function:
```C
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};

int misc_register(struct miscdevice *misc)
{
}
```

The most interesting field in the `miscdevice` struct is probably `fops`, which describes different operations that are allowed on the device. Following is the `struct file_operations` definition:
```C
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iterate) (struct file *, struct dir_context *);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*aio_fsync) (struct kiocb *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
			u64);
	ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
			u64);
};
```

This struct is huge! However, as we saw in the output from `readelf`, this misc device probably only has `read()`, `write()`, and `flush()` implemented.

We can start by looking at `flush()`:
```asm
push rbp
xor eax, eax
mov rbp, rsp
pop rbp
ret
```
Nothing special here, it only returns 0.


```C
__int64 tg_init()
{
  if ( byte_121A & 4 )
    _dynamic_pr_debug(descriptor_28886, "helloooo\n");
  return misc_register(&tg_dev);
}
```

The interesting function call here is `misc_register()`. See [lxr](http://elixir.free-electrons.com/linux/latest/source/drivers/char/misc.c#L186) for the complete function definition. It takes a `struct miscdevice *` as the only argument.

```C
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};
```

If we take a look at the pointer passed to `misc_register()`, we see that the name of the device is "tghack", and that `fops` points to a `file_operations` struct called `tg_fops`. Based on the information in these two structs we know that the driver registers a misc device at `/dev/tghack`, and the supported functions are `read()`, `write()`, and `flush()`. `flush()` does nothing except returning 0, so we can ignore this function.

`read()` is a lot more simple than `write()`, so we start by analyzing this function. A simple way to test it is interacting with the device:
```
$ cat /dev/tghack
Password please!
```

Looking at the assembly or disassembly, we can see that `read()` doesn't do anything else. Let's move on to `write()`. This function is called whenever we write to the device, and probably validates the password. Following is a cleaned-up version of the checking code:

```C
// 8 x 4 matrix
do {
	pos = cnt + 4;
	ptr = matrix_ptr;
	do {
		v23 = cnt++;
		++ptr;
		*(ptr - 1) = *((char *)&v37 + v23 - 304);
	} while (pos != cnt);
	matrix_ptr += 4;                          // next row
	cnt = pos;
} while (pos != 32);

ret = 1;
if (matrix[1] - 3 != matrix[3]) ret = 0;
if (matrix[0] != matrix[28]) ret = 0;
if (matrix[3] != matrix[10]) ret = 0;
if (matrix[16] != matrix[19]) ret = 0;
if (matrix[23] + 2 != matrix[24]) ret = 0;
if (matrix[25] + matrix[26] != 106) ret = 0;
if (matrix[24] != matrix[25]) ret = 0;
if (matrix[20] - matrix[16] != 15) ret = 0;
if (matrix[28] - matrix[24] != 44) ret = 0;
if (matrix[8] + 17 != matrix[9]) ret = 0;
if (matrix[0] != 95) ret = 0;
if (matrix[1] - 1 != matrix[5]) ret = 0;
if (matrix[2] - matrix[3] != 5) ret = 0;
if (matrix[12] != 114) ret = 0;
if (matrix[13] != 95) ret = 0;
if (matrix[31] != 121) ret = 0;
if (matrix[10] != matrix[31] - 9) ret = 0;
if (matrix[4] != matrix[11]) ret = 0;
if (matrix[16] != matrix[4]) ret = 0;
if (matrix[21] != matrix[22]) ret = 0;
if (matrix[21] + matrix[22] != 190) ret = 0;
if (matrix[21] != matrix[6]) ret = 0;
if (matrix[22] != matrix[7]) ret = 0;
if (matrix[11] - matrix[0] != 6) ret = 0;
if (matrix[4] != matrix[8] + 1) ret = 0;
if (matrix[30] + matrix[29] - matrix[31] != 87) ret = 0;
if (matrix[4] != matrix[30]) ret = 0;
if (matrix[1] != matrix[15]) ret = 0;
if (matrix[20] - 2 != matrix[18]) ret = 0;
if (matrix[19] != matrix[30]) ret = 0;
if (matrix[7] != matrix[27]) ret = 0
if (matrix[7] != matrix[14]) ret = 0;
if (matrix[17] != 99) ret = 0;
/* success! */
```

The matrix-generation code looks a bit weird, but it simply fills the matrix row-by-row with the user-supplied input. Because there are a lot of relationships between different positions in the matrix, we need some way to represent all of the checks. One way is to represent all the different if-tests as *constraints*. Microsoft research has a fantastic tool called Z3, which we can use to solve such constraints. For an introduction to Z3, check out [this tutorial](https://ericpony.github.io/z3py-tutorial/guide-examples.htm).

To find the valid password, we create the following python script:
```python
from z3 import *

def bv2int(bv):
    return int(str(bv))

matrix = [BitVec(str(i), 32) for i in range(32)]

s = Solver()

s.add(matrix[1] - 3 == matrix[3])
s.add(matrix[0] == matrix[28])
s.add(matrix[3] == matrix[10])
s.add(matrix[16] == matrix[19])
s.add(matrix[23] + 2 == matrix[24])
s.add(matrix[25] + matrix[26] == 106)
s.add(matrix[24] == matrix[25])
s.add(matrix[20] - matrix[16] == 15)
s.add(matrix[28] - matrix[24] == 44)
s.add(matrix[8] + 17 == matrix[9])
s.add(matrix[0] == 95)
s.add(matrix[1] - 1 == matrix[5])
s.add(matrix[2] - matrix[3] == 5)
s.add(matrix[12] == 114)
s.add(matrix[13] == 95)
s.add(matrix[31] == 121)
s.add(matrix[10] == matrix[31] - 9)
s.add(matrix[4] == matrix[11])
s.add(matrix[16] == matrix[4])
s.add(matrix[21] == matrix[22])
s.add(matrix[21] + matrix[22] == 190)
s.add(matrix[21] == matrix[6])
s.add(matrix[22] == matrix[7])
s.add(matrix[11] - matrix[0] == 6)
s.add(matrix[4] == matrix[8] + 1)
s.add(matrix[30] + matrix[29] - matrix[31] == 87)
s.add(matrix[4] == matrix[30])
s.add(matrix[1] == matrix[15])
s.add(matrix[20] - 2 == matrix[18])
s.add(matrix[19] == matrix[30])
s.add(matrix[7] == matrix[27])
s.add(matrix[7] == matrix[14])
s.add(matrix[17] == 99)

print s.check()

m = s.model()

dec = "".join([chr(bv2int(m[matrix[i]])) for i in range(32)])
print dec
```

To construct the checks, we changed all the `!=` statements to `==` to make them into valid constraints for Z3. Running the script gives us the password:
```
$ python2 solve.py
_super__duper__secret__1337__key
```

However, when writing the password to the device, we don't get anything back except a zero return code on success. Note that we need to send the password without a newline at the end.
```
$ echo "_super__duper__secret__1337__key" > /dev/tghack
-bash: echo: write error: Invalid argument
crap@ubuntu:~$ echo -n "_super__duper__secret__1337__key" > /dev/tghack
crap@ubuntu:~$ echo $?
0
```

Looking at the driver a bit more, we see that it calls `simple_read_from_buffer()` when the password is correct. This is the same function used in `tg_read()`, which means that the driver will send data back to us in the same buffer as the password is located in!

```C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd;
	char buf[256] = { 0 };
	const char *pass = "_super__duper__secret__1337__key";

	fd = open("/dev/tghack", O_RDWR);
	if (fd == -1) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	strcpy(buf, pass);

	if (write(fd, buf, strlen(buf)) <= 0) {
		perror("write()");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("yay! flag: %s\n", buf);

	close(fd);
	return 0;
}
```

```
$ gcc solve.c -o solve && ./solve
yay! flag: TG18{st0p_m4king_k3rn3l_m0dul3s_pl34s3_:(}
```
