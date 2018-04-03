#!/usr/bin/python3
from scapy.all import *

def pktanalyzer(pkt):
    print(pkt.summary())
    if pkt.haslayer(Raw):
        print(pkt[Raw])


data = 'Can i haz flagz now plz'
send(IP(dst="167.99.35.80")/TCP(dport=1337, sport=45034, flags='SU',reserved=1)/Raw(load=data))

sniff(iface='enp0s25',prn=pktanalyzer,filter="port 31337",store=0)
