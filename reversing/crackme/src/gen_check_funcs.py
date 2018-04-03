from os import urandom

flag = "TG18{jump_up_jump_up_and_get_d0wn!}"

template = """
static __attribute__((noinline)) int check_{}(unsigned char c)
{{
    return ({} & 0xff) == ({} & 0xff);
}}
"""

print "#define FLAG_SIZE {}".format(len(flag))

for i in range(len(flag)):
    c = flag[i]

    tmp = ord(urandom(1)) % 3

    if tmp == 0: # minus
        val = ord(urandom(1))
        while val > ord(c):
            val = ord(urandom(1))
        tmp_str = "(c - {})".format(val)
        print template.format(i, tmp_str, ord(c) - val)
    elif tmp == 1: # plus
        val = ord(urandom(1))
        while val < ord(c):
            val = ord(urandom(1))
        tmp_str = "(c + {})".format(val)
        print template.format(i, tmp_str, ord(c) + val)
    elif tmp == 2: # no op
        print template.format(i, "c", ord(c))

print "static int (*check_funcs[])(unsigned char c) = {"
for i in range(len(flag)):
    print "\tcheck_{},".format(i)

print "\tNULL\n};"
