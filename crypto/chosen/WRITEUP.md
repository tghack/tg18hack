# The Chosen One Writeup

We are provided with the source code in this challenge, so the first step should be to take a look at it:

```python
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
```

The code performs AES encryption in ECB mode: `cipher = AES.new(key, AES.MODE_ECB)`. AES-ECB is a symmetric cipher that encrypts 16-byte blocks of plaintext into 16-byte blocks of ciphertext. ECB is considered insecure, and should not be used in practice. One of the reasons it is considered insecure is that equal plaintexts result in the same ciphertext.

To attack such a cipher, we can perform what is called a chosen-plaintext attack. The task description also hints to this type of attack (The **Chosen** One). In this attack model, we can obtain ciphertexts for arbitrary plaintexts. We can see that this is true because the `enc` function takes our input and concatenates (`||`) it with other text. The encryption looks like this:

```
AES-ECB(pad("Hello, " || input || "! You are the chosen one. Here's the flag: " || flag))
```

The flag is unknown, but we control the input in the middle.

Before moving on, we need to take a look at the padding function:
```python
def pad(m):
    m = m + '1'

    while len(m) % 16 != 0:
        m += '0'
    return m
```

Since we are working with a block cipher, the plaintext has to be padded up to a multiple of 16 (the blocksize) before encryption. Here, a `1` plus as many `0`s as necessary to reach a 16-byte boundary is added to the plaintext before encryption.

The padding is very important for attacking this cipher. Since we can control part of the input, it is possible to control the padding at the end of the message. We will start by padding the first part of the message up to 16 bytes like this: `Hello, AAAAAAAAA`. Now we append a full block of padding like this: `1000000000000000`.

At this point, we known that the second block of the ciphertext is tied to our fake padding block. We can repeatedly add padding to the end of our input, until the last block of the ciphertext matches the second block. This means that the flag will end on a 16-byte boundary.

The question is then, what is the point of doing this? The second block is our chosen plaintext, which we can use to compute arbitrary ciphertexts. By controlling the padding after this block, we can repeatedly "push" one byte of the flag into the padding block at the end. In addition, we can compute all valid ciphertexts for each byte that we push off the end. For the first step, we push one byte of the flag into the padding block, resulting in the following plaintext block: `?100000000000000`, where the `?` is an unknown character (probably `}` since this is the last character of the flag). In our chosen-plaintext block, we compute all ciphertexts for printable characters plus the padding.

See the following code for the implementation:

```python
# pad up to nearest 16-byte block
# 1 character at position 9 that will be replaced each time
# padding at the end to align the flag at a 16-byte boundary
chosen_plaintext = bytearray("A"*9 + "1" + "0"*15 + "A"*pad_len)

log.info("Using padding block to find first 15 bytes of the flag...")
for check in range(0, 16):
    chosen_plaintext.insert(9, "_")
    for guess in printable:
        chosen_plaintext[9] = guess

        blocks = split(enc(chosen_plaintext))

        guess_block = blocks[1]
        final_block = blocks[-1]

        if final_block == guess_block:
            flag = guess + flag
            break
```

To summarize, we need to do the following:

1. find the padding length
2. find the first 15 characters of the flag
3. find the rest of the flag

After we have found 15 characters, we can't use the padding at the end anymore, and thus have to use the already-known characters of the flag.

For the first step, we send our fake block of padding plus some additional padding and check to see if the second and final blocks match, meaning that we have a full block of padding at the end of the ciphertext.

```python
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
```

Now we have to use the padding at the end to brute-force every byte of the flag:

```python
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
```


Finally, we can brute-force the remaining characters by comparing the eight and second blocks of ciphertext.
```python
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
```

The full solution can be found [here](src/solve.py)
```
$ python2 solve.py
[*] Calculating padding...
[+] Found padding length: 14
[*] Using padding block to find first 15 bytes of the flag...
[+] First part of the flag: he_chosen_one!}
[*] Can't use padding anymore, using known flag...
[+] Yay! TG18{hell0_neo_you_are_the_chosen_one!}
```
