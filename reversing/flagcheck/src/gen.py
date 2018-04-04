import binascii
from os import urandom
from hashlib import sha512

flag = "TG18{g0_g0_g0_w3_h4v3_t0_ch3ck_m0r3_fl4gs}"

sums = []
tmp = ""
for i in range(len(flag)):
    tmp += flag[i]
    sums.append(sha512(tmp).digest())

keys = []
for i in range(len(flag)):
    keys.append(urandom(64))

def do_xor(data, key):
    out = bytearray()
    for i in range(len(data)):
        out.append(ord(data[i]) ^ ord(key[i]))

    return out

enc = []
assert len(sums) == len(keys)
for i in range(len(sums)):
    enc.append(do_xor(sums[i], keys[i]))

def do_escape(s):
    assert (len(s) % 2) == 0
    out = ""
    for i in range(0, len(s), 2):
        out += "\\x"
        out += s[i:i + 2]

    return out

f = open("check.h", "w+")
f.write("#ifndef __CHECK_H\n")
f.write("#define __CHECK_H\n\n")

encrypted = "static char *encrypted[] = { "
for e in enc:
    h = binascii.hexlify(e)
    encrypted += "\"" + do_escape(h) + "\",\n"
encrypted += "};\n"
f.write(encrypted)

key_decl = "static char *keys[] = { "
for k in keys:
    h = binascii.hexlify(k)
    key_decl += "\"" + do_escape(h) + "\",\n"
key_decl += "};\n"
f.write(key_decl)

f.write("\n#endif /* __CHECK_H */\n")
f.close()
