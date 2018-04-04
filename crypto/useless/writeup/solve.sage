import socket
import binascii

def remote(hostname, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((hostname, port))
    return s

def recvuntil(s, msg):
    buf = ""
    while not buf.endswith(msg):
        buf += s.recv(1)

def recvline(s):
    tmp = ""
    line = ""
    while tmp != "\n":
        tmp = s.recv(1)
        line += tmp

    return line

def get_state(s):
    recvuntil(s, "> ")
    s.send("1\n")
    recvuntil(s, "you: ")
    return int(recvline(s)[:-1])

def encrypt(s):
    recvuntil(s, "> ")
    s.send("2\n")
    recvuntil(s, "go: ")
    return int(recvline(s)[:-1])

s = remote("localhost", 4444)

# recover lcg
s0 = get_state(s)
s1 = get_state(s)
s2 = get_state(s)
s3 = get_state(s)
print "server s1: {}".format(s0)
print "server s2: {}".format(s1)
print "server s3: {}".format(s2)
print "server s4: {}".format(s3)

m = 325010924826975202081527196997

# recover a and b from the lcg: ((a * s_n) + b) % m
if gcd((s1 - s0), m) == 1:
    a = (s2 - s1) * inverse_mod((s1 - s0), m)
elif gcd((s2 - s0), m) == 1:
    a = (s3 - s1) * inverse_mod((s2 - s0), m)
else:
    print "failed :("

a = a % m
b = (s1 - (s0 * a)) % m
print "a: {}".format(a)
print "b: {}".format(b)

# recover initial state, so we can compute all values of the lcg
s_init = (((s0 - b) % m) * inverse_mod(a, m)) % m
print "initial state: {}".format(s_init)

# https://crypto.stackexchange.com/questions/30884/help-understanding-basic-franklin-reiter-related-message-attack
def my_gcd(a, b):
    return a.monic() if b == 0 else my_gcd(b, a % b)

n = 20161106930940520616016565054651922934902681677479325746590602620373385360069982971405845432120820667058057601082859165287585534058466344572190110259313686370581231514050738184276784517607812027635675629105186141526093986560636577001906412301326369220631238246247360250309825556376993212983024795009212230155562571688025630911340360704138715077125407384757949516178958461483096887573924289162249441189464369276557139767377577477257141862608150251441002084405026698636165824420980606373634628051236493510100799126956456401834845005440720706582078472059830562721744350646280393986962223567089670808650608224818542361553

class lcg():
    # initialize lcg with a state
    def __init__(self, a, b, s):
        self.m = 325010924826975202081527196997
        self.a = a
        self.b = b
        self.s = s

    def next(self):
        self.s = ((self.a * self.s) + self.b) % self.m
        return self.s
    
    def state(self):
        return self.s

l = lcg(a, b, s_init)
s0 = l.next()
s1 = l.next()
s2 = l.next()
s3 = l.next()
s4 = l.next()
s5 = l.next()
s6 = l.next()
print "s0: {}".format(s0)
print "s1: {}".format(s1)
print "s2: {}".format(s2)
print "s3: {}".format(s3)
print "s4: {}".format(s4)
print "s5: {}".format(s5)

c1 = encrypt(s)
c2 = encrypt(s)

print "c1: {}".format(c1)
print "c2: {}".format(c2)

# c1 should be the smallest ciphertext, so we might
# have to flip c1 and c2 + s4 and s5
if s4 > s5:
    tmp = c1
    c1 = c2
    c2 = tmp
    diff = s4 - s5
    flag_off = s5
else:
    diff = s5 - s4
    flag_off = s4

R.<X> = Zmod(n)[]
f1 = X^3 - c1
f2 = (X + diff)^3 - c2

print "diff: {}".format(diff)

flag_plus_s4 = int(- my_gcd(f1, f2).coefficients()[0])
print "flag+s4: {}".format(flag_plus_s4)
flag = flag_plus_s4 - flag_off

print "flag: {}".format(binascii.unhexlify(hex(flag)))

s.close()
