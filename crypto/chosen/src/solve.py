from pwn import *
from string import printable

def split(m):
    assert(len(m) % 32 == 0)

    blocks = []
    for i in range(0, len(m), 32):
        blocks.append(m[i:i + 32])

    return blocks

def enc(user):
    with context.local(log_level = "ERROR"):
        r = remote("chosen1.tghack.no", 1999)

        r.recvuntil("name? ")
        r.sendline(user)

        line = r.recvline()[:-1]
        r.close()
    return line

log.info("Calculating padding...")
chosen_plaintext = "A"*9 + "1" + "0"*15
for check in range(0, 16):
    chosen_plaintext += "A"
    blocks = split(enc(chosen_plaintext))
    pad_block = blocks[1]
    final_block = blocks[-1]
    if final_block == pad_block:
        break

pad_len = len(chosen_plaintext) - 25
log.success("Found padding length: {}".format(pad_len))

chosen_plaintext = bytearray("A"*9 + "1" + "0"*15 + "A"*pad_len)
flag = ""

log.info("Using padding block to find first 15 bytes of the flag...")
for check in range(0, 16):
    # placeholder, will be replaced with each guess
    chosen_plaintext.insert(9, "_")
    for guess in printable:
        chosen_plaintext[9] = guess

        blocks = split(enc(chosen_plaintext))

        guess_block = blocks[1]
        final_block = blocks[-1]

        if final_block == guess_block:
            flag = guess + flag
            break

log.success("First part of the flag: {}".format(flag))
log.info("Can't use padding anymore, using known flag...")

while "TG18" not in flag:
    chosen_plaintext = bytearray("A"*9 + "0" + flag + "A"*14)
    for guess in printable:
        chosen_plaintext[9] = guess
        blocks = split(enc(chosen_plaintext))
        if len(blocks) < 8:
            continue
        guess_block = blocks[1]
        final_block = blocks[7]
        if final_block == guess_block:
            flag = guess + flag
            break

log.success("Yay! " + flag)
