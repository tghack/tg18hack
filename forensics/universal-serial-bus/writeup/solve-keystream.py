#!/usr/bin/python3
from scapy.all import *

KEY_CODES = {
4:"A",
5:"B",
6:"C",
7:"D",
8:"E",
9:"F",
10:"G",
11:"H",
12:"I",
13:"J",
14:"K",
15:"L",
16:"M",
17:"N",
18:"O",
19:"P",
20:"Q",
21:"R",
22:"S",
23:"T",
24:"U",
25:"V",
26:"W",
27:"X",
28:"Y",
29:"Z",
30:"1",
31:"2",
32:"3",
33:"4",
34:"5",
35:"6",
36:"7",
37:"8",
38:"9",
39:"0",
40:"\n",
41:"'ESC'",
42:"'DELETE(backspace)'",
44:" ",
45:"-",
46:"=",
47:"{",
48:"}",
51:";",
54:"<",
55:">",
56:"/",
}
 
pkts = rdpcap("usb.pcapng")
keystrokes= ""
for packet in pkts:
    if packet:
        #print(packet.show())
        hid_report = packet.load[-8:]
        key_code = hid_report[2]
        key = KEY_CODES.get(key_code, False)
        if key:
            keystrokes += key
 
print(keystrokes)
