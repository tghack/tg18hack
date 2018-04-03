import binascii
from Crypto.Cipher import AES

print "yay"

key = "this_is_a_super_s3cret_key_right"
key = "_super__duper__secret__1337__key"
print len(key)
assert len(key) == 32

obj = AES.new(key, AES.MODE_CBC, "\x00"*16)
message = "TG18{st0p_m4king_k3rn3l_m0dul3s_pl34s3_:(}"
message += "\x00" * (64 - len(message))
print len(message)
ciphertext = obj.encrypt(message)
print "ciphertext: {}".format(binascii.hexlify(ciphertext))
#obj2 = AES.new(key, AES.MODE_CBC, "\x00"*16)
#print obj2.decrypt(ciphertext)
