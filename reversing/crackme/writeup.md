# Writeup for crackme

Let's run the binary!
```
C:\Users\Christian REsell\Desktop>crackme.exe
Password: flagpls
nope!
```

So our goal is to find the correct password, let's start reversing!

By cross-referencing strings in IDA, we can quickly find the main function which starts at `0x401A79`.
Looks like the program uses some trick to jump to either "yay" or "nope". The last function called is the one at `0x4019F6`, so let's check that one out.

The first piece of code in this function checks the length of the input:
```
loc_401A02:
mov     eax, [ebp+arg_0]
mov     [esp+28h+Str], eax ; Str
call    strlen
cmp     eax, 23h
```

Which tells us that the password is 0x23 = 35 characters long.

The next part is probably the most complicated. The program checks the input, character by character, and feeds it to a function located in the array at `0x403040`.
This array contains 35 functions, one for each character in the input.

Luckily for us, the functions are located sequentially in the binary. Starting at `0x401560` and ending at `0x401971`.

Here's an example of one of these functions:
```
.text:00401560 sub_401560      proc near               ; CODE XREF: sub_4019F6+5Cp
.text:00401560                                         ; DATA XREF: .data:off_403040o
.text:00401560
.text:00401560 var_4           = byte ptr -4
.text:00401560 arg_0           = dword ptr  8
.text:00401560
.text:00401560                 push    ebp
.text:00401561                 mov     ebp, esp
.text:00401563                 sub     esp, 4
.text:00401566                 mov     eax, [ebp+arg_0]
.text:00401569                 mov     [ebp+var_4], al
.text:0040156C                 cmp     [ebp+var_4], 54h
.text:00401570                 setz    al
.text:00401573                 movzx   eax, al
.text:00401576                 leave
.text:00401577                 retn
.text:00401577 sub_401560      endp
```

This function simply checks the character passed as the first argument against 0x54 (hex value of the ASCII character T). However, some functions subtract or add to the character before performing the check.

I exported the binary data of all the functions to a text file, so I could do some analysis with capstone.

Thankfully, the check functions are very simple, so they can be parsed with Capstone without too much trouble.

```python
from capstone import *
from binascii import unhexlify

code = "5589E55DC390909090909090909090905589E583EC048B45088845FC807DFC540F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E81C0FB6C083F82B0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC310F94C00FB6C0C9C35589E583EC048B45088845FC807DFC380F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05E00000000FB6C083F85B0F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E80A0FB6C083F8600F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8800FB6C03DF50000000F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05E10000000FB6C083F84E0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC700F94C00FB6C0C9C35589E583EC048B45088845FC807DFC5F0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC750F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05A50000000FB6C083F8150F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E81E0FB6C083F8410F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8420FB6C083F8280F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05CC0000000FB6C083F8410F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8120FB6C083F85B0F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05A90000000FB6C083F8190F94C00FB6C0C9C35589E583EC048B45088845FC807DFC5F0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC750F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05850000000FB6C03DF50000000F94C00FB6C0C9C35589E583EC048B45088845FC807DFC5F0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC610F94C00FB6C0C9C35589E583EC048B45088845FC807DFC6E0F94C00FB6C0C9C35589E583EC048B45088845FC807DFC640F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E82F0FB6C083F8300F94C00FB6C0C9C35589E583EC048B45088845FC807DFC670F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8030FB6C083F8620F94C00FB6C0C9C35589E583EC048B45088845FC807DFC740F94C00FB6C0C9C35589E583EC048B45088845FC807DFC5F0F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8470FB6C083F81D0F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E81B0FB6C083F8150F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E8350FB6C083F8420F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC83E81B0FB6C083F8530F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC058B0000000FB6C03DAC0000000F94C00FB6C0C9C35589E583EC048B45088845FC0FB645FC05C10000000FB6C083F83E0F94C00FB6C0C9C3"
code = unhexlify(code)

dec = ""

val = 0
md = Cs(CS_ARCH_X86, CS_MODE_32)
for i in md.disasm(code, 0x1000):
    m = i.mnemonic
    if m == "add":
        val = -int(i.op_str.split(" ")[1], 16)
    elif m == "sub" and "esp" not in i.op_str:
        sub = True
        val = int(i.op_str.split(" ")[1], 16)
    elif m == "cmp":
        cmp_val = int(i.op_str.split(", ")[1], 16)
        dec += chr((cmp_val + val) & 0xff)
        val = 0

    #print("0x%x:\t%s\t%s" %(i.address, i.mnemonic, i.op_str))

print "flag: {}".format(dec)
```

Running it prints the flag:
```
$ python2 solve.py
flag: TG18{jump_up_jump_up_and_get_d0wn!}
```
