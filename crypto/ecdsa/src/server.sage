from sage.all import *
import sys
import hashlib
from flag import flag

message = "I swear I'm Alice, now give me the flag!"
p = 39402006196394479212279040100143613805079739270465446667948293404245721771496870329047266088258938001861606973112319
n = 39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942643
seed = 0xa335926aa319a27a1d00896a6773a4827acdac73
c = 0x79d1e655f868f02fff48dcdee14151ddb80643c1406d0ca10dfe6fc52009540a495e8042ea5f744f6e184667cc722483
a = -3
b = 0xb3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef
Gx = 0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7
Gy = 0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f
N = int(n).bit_length()
E = EllipticCurve(GF(p), [a,b])
G = E(Gx, Gy)
Qax = 32328635860298239170667609951989582258221921247035930895445266104879324530339762372381205642669609129189280443043904 
Qay = 11206344354656197095717691611166578675359573002212769066491543658371429575422173318994300502286639114462919746722610
Qa = E(Qax, Qay)


def validateSignature(m, sig):
    r = sig[0]
    s = sig[1]
    if s < 1 or s >= n:
        return False
    if r < 1 or r >= n:
        return False

    e = int(hashlib.sha256(m).hexdigest(), 16)
    w = (s**-1) % n
    u1 = e*w % n
    u2 = r*w % n
    Qvalidate = (u1 * G) + (u2 * Qa)
    if (r % n) == (int(Qvalidate[0]) % n):
        return True
    
    return False



def main():

    print("Hello Alice!")
    print("I will give you the flag, but first I have to verify that you indeed are Alice.")
    print("Please sign the following message: \"" + message + "\"")

    while True:
        sys.stdout.write("\nr: ")
        sys.stdout.flush()
        r = raw_input("")
        if not r.isdigit():
            print("Invalid r!")
            continue

        r = sage.rings.integer.Integer(r)
        sys.stdout.write("\ns: ")
        sys.stdout.flush()
        s = raw_input("")
        if not s.isdigit():
            print("Invalid s!")
            continue

        s = sage.rings.integer.Integer(s)
        if validateSignature(message, (r, s)):
            print("Signature validated OK!")
            print("Here's your flag, Alice: " + flag)
            sys.exit()
        else:
            print("Invalid signature!")


if __name__ == "__main__":
    try:
        main()
    except:
        print("\nBye! :)\n")
