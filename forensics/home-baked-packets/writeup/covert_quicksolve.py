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
