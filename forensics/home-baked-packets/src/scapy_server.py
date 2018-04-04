#!/usr/bin/python3

from scapy.all import *
import os
import re
import time

#flag="TG18{Finally-s0me-fr3sh-p4ck3ts!}"
flag="TG18{testflagg}"

flag = IP()/UDP(dport=31337,sport=50000)/Raw(load=flag)

def return_flag(dstip):
    print("Result sendt to: " + dstip )
    returnpkt = flag
    returnpkt[IP].dst=dstip
    time.sleep(1)
    send(returnpkt)


def pktanalyzer(pkt):
    if pkt.haslayer(TCP):
        print("pkt: " + pkt.summary())
        if pkt[TCP].flags == 34:
            print("FLAGS OK")
            if pkt[TCP].reserved==1:
                print("TCP reserved OK")
                if pkt.haslayer(Raw):
                    if "can i haz flagz now plz" in str(pkt[Raw]).lower():
                        print("Another solved!!")
                        return_flag(pkt[IP].src)


if __name__ == '__main__':
    sniff(iface='eth0',prn=pktanalyzer,filter="tcp and dst port 1337",store=0)

