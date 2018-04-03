# Writeup Home-Baked-Packets

You start by analyzing the first pcap: home-baked-transfer.pcapng.

This first step is a trivial http file extraction that you can do with wireshark:

```
1. Rightclick on one of the http packets, select follow http stream.
2. Note that the http requested file is named "covert"
3. In the new window, select "Show and save data as": Raw and save the raw packet stream in a file, for example named covert.
4. open the file in a text editor and remove the first 10 lines, as we only want the package data.
5. run file on the file to verify that it is recognized as a PCAP file
```
Now onto the second part:

This part is a _bit_ more tricky.
If we poke around we find that there are some packets in the stream from 10.53.39.207 that has the IP reserved bit flag set. This is also know as "Evilbit", or RFC 3514.

To quicly find any packets with evilbit set, you can use the wireshark displayfilter "ip.flags.rb == 1"
If we follow the only stream that has some packets with evilbit set, we see that not all packets have the flag set and if we extract it with a scapy python script we can see that it is infact a binary stream that we can convert to ascii to view the "callback" domain:

[covert_quicksolve.py](covert_quicksolve.py)
```python
#!/usr/bin/scapy

from scapy.all import *
import binascii

pkts = rdpcap("covert.pcap")
bitstream=""

for pkt in pkts:
    if pkt.haslayer(TCP):
        if pkt.haslayer(Raw):
            if pkt[IP].src == "10.53.39.207":
                if pkt[IP].flags==4:
                    bitstream += "1"
                elif pkt[IP].flags==0:
                    bitstream += "0"

print(bitstream)

lol=int('0b' + bitstream,2)
decoded=lol.to_bytes((lol.bit_length() + 7) // 8, 'big').decode()
print(decoded)
```

This gives the following output:
```
01100011001100100010111001110100011001110110100001100001011000110110101100101110011011100110111100101111010000010011010100110111001100100101001101111000011000110111011001100010001100010111101000110011
c2.tghack.no/A572Sxcvb1z3
```

If we download the file, we get a "packet baking recipe":
[A572Sxcvb1z3](A572Sxcvb1z3)


If we follow the instructions the flag will be returned on UDP port 31337:

[bakepacket.py](bakepacket.py)
```
#!/usr/bin/python3
from scapy.all import *

def pktanalyzer(pkt):
    print(pkt.summary())
    if pkt.haslayer(Raw):
        print(pkt[Raw])

data = 'Can i haz flagz now plz'
send(IP(dst="167.99.35.80")/TCP(dport=1337, sport=45034, flags='SU',reserved=1)/Raw(load=data))

sniff(iface='$INSERT YO INTERFACE HERE',prn=pktanalyzer,filter="port 31337",store=0)
```


Flag:
```
TG18{Finally-s0me-fr3sh-p4ck3ts!}
```
