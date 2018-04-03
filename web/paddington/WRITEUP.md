# Writeup [paddington](web/paddington)

We can start by opening the pcap in wireshark. The important packets are those marked as HTTP.
In the HTTP packet we find this header.

```
Cookie: User-Auth=757365723d61646d696e266578706972793d31353231373138353339-7d2250c3093c7ec2821739f45cd3f972048899dbf268d4608ccc090b71f6e735; Max-Age=300
```

We can see that the cookie is made up by two hexadecimal strings separated by a `-`.
If we decode the first part with xxd like this
```
echo 757365723d61646d696e266578706972793d31353231373138353339 | xxd -r -p -
```

we get

```
user=admin&expiry=1521718539
```

The expiry is given as a unix timestamp.

If we try to decode the second part of the cookie with xxd we get something that looks like garbage.
Based on the length of the hex string we can assume that it is in fact a sha256 sum.

We can exploit this using the [Length Extension Attack](https://en.wikipedia.org/wiki/Length_extension_attack).
The length extension attack makes it possible to append data to a message
while still being able to generate a new hash without knowing the salt.

A nice tool for working with length extension attacks is [hash\_extender](https://github.com/iagox86/hash_extender)
We don't know the length of the salt, but we can try different lengths until we get the correct one.
The [solve.sh](solve.sh) script tries different sizes of salt and gives us the token in return.
The token is `TG18{no_one_told_me_that_sha256_was_insecure}`
