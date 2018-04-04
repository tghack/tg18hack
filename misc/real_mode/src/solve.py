from pwn import *

r = remote("localhost", 8086)

r.recvuntil(": ")
loader = open("bin/loader.img", "rb").read()
assert(len(loader) == 512)
r.send(loader)

#print r.recvuntil("SeaBIOS")[-7:]
r.recvline()
r.interactive()
