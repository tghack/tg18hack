# Writeup Fans of Alice

The task description gives us two signatures, a public key, `SHA-256`,
some parameters, and a server we can connect to.

Let's try to connect to the server:
```bash
$ nc fansofalice.tghack.no 8888
Hello Alice!
I will give you the flag, but first I have to verify that you indeed are Alice.
Please sign the following message: "I swear I'm Alice, now give me the flag!"

r: 123123

s: 123123
Invalid signature!

r:
```

The server wants Alice to sign the message `I swear I'm Alice, now give me the flag!`,
but we don't have her private key. Is there a way to find it?

We can start by trying to identify the signature scheme she is using. In the
`parameters.txt` file we have the reference `Curve P-384` and a bunch of parameters.
Curve? As in elliptic curves? A quick look at 
[Wikipedia](https://en.wikipedia.org/wiki/Elliptic-curve_cryptography#Domain_parameters)
confirmes our suspicion. We have all the parameters needed for some delicious
elliptic-curve cryptography.

Google `elliptic curve signature` (or click [here](http://bfy.tw/HCKB)) and one of
the top results will be 
[Elliptic Curve Digital Signature Algorithm](https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm).
The signatures of this signature scheme consists of the pair `(r, s)`, which is 
exactly what the server asks for. Interesting! What's even more interesting is 
the following sentence:
> As the standard notes, it is not only required for `k` to be secret, but it 
> is also crucial to select different `k` for different signatures, otherwise 
> the equation in step 6 can be solved for `d_A`, the private key.

Since we are given two signatures, we can assume that those two messages
have been signed with the same `k`. The Wikipedia article also describes how to
find `k` and the private key, implemented in the following `sage` code:
```python
message1 = "The Gathering"
sigMessage1 = (19141937439545319859705988641798784719777074439076362411205035832172647233987084496949380211549289201431140681830583, 30790442289912604372026466305385930790012147668887228944126016056323471535177909584124933469552473283807866733738134)
message2 = "Social Reality"
sigMessage2 = (19141937439545319859705988641798784719777074439076362411205035832172647233987084496949380211549289201431140681830583, 17166674590681927265388369644497902549253241028403678049750759342541880555990289951008235479676445852928448289263732)

def findPrivateKey(m1, sig1, m2, sig2):
    if sig1[0] != sig2[0]:
        return "error, not same k"

    r = sig1[0]
    s1 = sig1[1]
    s2 = sig2[1]
    e1 = int(hashlib.sha256(m1).hexdigest(), 16)
    e2 = int(hashlib.sha256(m2).hexdigest(), 16)
    k = (e1 - e2)/(s1 - s2) % n
    priv = (s1 * k - e1)/r % n
    return (k, priv)

print(findPrivateKey(message1, sigMessage1, message2, sigMessage2))
```
```bash
$ sage findPrivateKey.sage
(34567, 23954279743199567697938911451558038948789132779702252316913770526556128643257362374213062855183321703967582476494167)
```
Success! We now have both `k` and the private key and can sign the
message we get from the server. Follow the instructions on Wikipedia
(or use a library of your choice) and create the signature:
```
(19141937439545319859705988641798784719777074439076362411205035832172647233987084496949380211549289201431140681830583, 5958298771862043732124279578692013853521791150003173681505062200492339520514093500912389565727934468304471771631300)
```
Then we just have to feed it to the server:
```bash
$ nc fansofalice.tghack.com 8888
Hello Alice!
I will give you the flag, but first I have to verify that you indeed are Alice.
Please sign the following message: "I swear I'm Alice, now give me the flag!"

r: 19141937439545319859705988641798784719777074439076362411205035832172647233987084496949380211549289201431140681830583

s: 5958298771862043732124279578692013853521791150003173681505062200492339520514093500912389565727934468304471771631300
Signature validated OK!
Here's your flag, Alice: TG18{t0_be_or_n0t_to_b3_r4ndom}

Bye! :)

```
