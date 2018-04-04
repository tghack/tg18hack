import sys
import time
import random

from flag import flag

e = 3
n = 20161106930940520616016565054651922934902681677479325746590602620373385360069982971405845432120820667058057601082859165287585534058466344572190110259313686370581231514050738184276784517607812027635675629105186141526093986560636577001906412301326369220631238246247360250309825556376993212983024795009212230155562571688025630911340360704138715077125407384757949516178958461483096887573924289162249441189464369276557139767377577477257141862608150251441002084405026698636165824420980606373634628051236493510100799126956456401834845005440720706582078472059830562721744350646280393986962223567089670808650608224818542361553

class lcg():
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

def main():
    t = time.time()
    random.seed(t)

    a = random.getrandbits(80)
    b = random.getrandbits(80)
    s = random.getrandbits(80)
    l = lcg(a, b, s)

    while True:
        print "1. check state"
        print "2. encrypt flag"
        print "3. exit"
        sys.stdout.write("> ")
        sys.stdout.flush()
        choice = int(raw_input(""))

        if choice == 1:
            print "state for you: {}".format(l.next())
            sys.stdout.flush()
        elif choice == 2:
            tmp = l.next()
            c = pow(flag + tmp, e, n)
            print "here you go: {}".format(c)
            sys.stdout.flush()
        elif choice == 3:
            sys.exit()
        else:
            print "Invalid choice!"

if __name__ == "__main__":
    main()
