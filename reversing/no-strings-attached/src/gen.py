import binascii
key = "z3r0_c00l_w4s_h3r3"
flag = "TG18{I_w4s_ly1ng_ab0ut_th3_str1ngs}"

enc = "".join([chr(ord(flag[i]) ^ ord(key[i % len(key)])) for i in range(len(flag))])

print binascii.hexlify(enc)
