# Writeup Mysterious message

We are given the following information: 
```python
n = 27606985387162255149739023449107931668458716142620601169954803000803329
e = 23664470390918201581474870981830633334318368330153879258574426167492943
c = 7290791170374533506623294411228106680208267451537289851738564756180552
```

So what's this? Since this is in the cryptography category, it's probably
some parameters for an encryption scheme and some encrypted data. `c` is 
normally used as the name for cipher texts, so we assume that this is the
case here as well. That leaves us with `n` and `e`. If we search the web
for some of the more well known encryption schemes,  we will see that the
public key for [RSA](https://en.wikipedia.org/wiki/RSA_(cryptosystem))
consists of `n` and `e`. 

Looks like we have a message encrypted with an RSA public key. But we don't
have the private key to decrypt the message. By reading a bit on Wikipedia, 
we can see that we can break RSA by factoring `n` into the primes `p` and
`q`. If `n` is 1024 bits or higher, no efficient method is known. But the
`n` we are given is not that big:
```python
>>> n = 27606985387162255149739023449107931668458716142620601169954803000803329
>>> n.bit_length()
234
```
Let's try to factor it. There's a handy method called `factor` in `sage`
that we can use (it might take some time to complete):
```python
sage: n = 27606985387162255149739023449107931668458716142620601169954803000803329
sage: factor(n)
162259276829213363391578010288127 * 170141183460469231731687303715884105727
```
Success! We now have `p` and `q`. We can use these to generate the private
key and decrypt the message.

The following `sage` code sets up the RSA system and decrypts the message
(see [here](https://doc.sagemath.org/html/en/thematic_tutorials/numtheory_rsa.html)
for more information on RSA in `sage`):
```python
import binascii

p = 162259276829213363391578010288127
q = 170141183460469231731687303715884105727
e = 23664470390918201581474870981830633334318368330153879258574426167492943
c = 7290791170374533506623294411228106680208267451537289851738564756180552
n = p*q

# Create private key
phi = (p-1) * (q-1)
bezout = xgcd(e, phi)
d = Integer(mod(bezout[1], phi))

# Decrypt message
m = power_mod(c, d, n)
print(binascii.unhexlify(hex(m)))
```
Run it:
```bash
$ sage solve.sage
TG18{use_large_primes_pls}
```
