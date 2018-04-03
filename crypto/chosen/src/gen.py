# generate key
import random
import binascii

random.seed()

r = random.getrandbits(16 * 8)

hexbuf = hex(r)[2:-1]
print hexbuf

open("key.bin", "wb+").write(binascii.unhexlify(hexbuf))
