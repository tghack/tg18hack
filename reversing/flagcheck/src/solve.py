import sys
import string
from subprocess import Popen, PIPE, STDOUT

pindir = "./pin-3.5-97503-gac534ca30-gcc-linux/"
pin = pindir + "pin"
inscount = pindir + "source/tools/ManualExamples/obj-intel64/inscount0.so"
prog = "./" + sys.argv[1]

def get_count(try_flag):
    cmd = [pin, "-injection", "child", "-t", inscount , "--" , prog]
    p = Popen(cmd, stdout=PIPE, stdin=PIPE, stderr=STDOUT)
    print "trying {}".format(try_flag)
    stdout = p.communicate(try_flag + "\n")

    with open("inscount.out", "r") as f:
        data = f.read()

    count = int(data[6:])
    return count

max_count = 0
flag = "TG18{"
cur = 0
max_count = get_count(flag + "A"*(41 - len(flag)) + "}")

print "initial max_count: {}".format(max_count)
alphabet = "_{}"
alphabet += "0123456789abcdefghijklmnopqrstuvwxyz"

tmp = ""
candidate = ""
while len(flag) < 42:
    for cur in alphabet:
        try_flag = flag + cur + ("A" * (41 - len(flag) - len(cur))) + "}"
        count = get_count(try_flag)

        # found a new character!
        if count > max_count:
            max_count = count
            #print "found new candidate {}".format(cur)
            #print "max count {}".format(max_count)
            #flag += cur
            #break
            candidate = cur
    flag += candidate
    print "flag: {}".format(flag)
