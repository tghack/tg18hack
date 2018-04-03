import sys
import binascii
from Crypto.Cipher import AES

flag = open("flag.txt").read().strip("\n")
key = open("key.bin", "rb").read()

def pad(m):
    m = m + '1'

    while len(m) % 16 != 0:
        m += '0'
    return m

def enc(user):
    cipher = AES.new(key, AES.MODE_ECB)

    sys.stderr.write("user: {}".format(user) + "\n")
    m = "Hello, " + user + "! You are the chosen one. Here's the flag: " + flag

    sys.stderr.write("message: {}".format(pad(m)) + "\n")
    return binascii.hexlify(cipher.encrypt(pad(m)))

sys.stdout.write("Hey! What's your name? ")
sys.stdout.flush()
user = raw_input("")

enc = enc(user)
print enc
