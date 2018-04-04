#!/usr/bin/python3

from scapy.all import *
import random
import hashlib
from random import uniform
import binascii


cache=[]

flag="c2.tghack.no/A572Sxcvb1z3"

binflag=list(bin(int.from_bytes(flag.encode(), 'big')))
del binflag[0]

#DEBUG:

#print("[+] DEBUG: Current flag: \t\t" + flag)
#print("[+] DEBUG: Flag length: \t\t" + str(len(flag)))
#print("[+] DEBUG: Binary flag: \t\t" + str(binflag))
#print("[+] DEBUG: Binary flag length: \t\t"  + str(len(binflag)))

#__import__("IPython").embed()


pkttime=1500000000
covert=[]


def packet_time(pkt):
    global pkttime
    pkt.time=pkttime + uniform(0.001, 0.5)
    pkttime=pkt.time
    return

def xor_str(a,b):
    xored = []
    for i in range(max(len(a), len(b))):
        xored_value = ord(a[i%len(a)]) ^ ord(b[i%len(b)])
        xored.append(hex(xored_value)[2:])
    return ''.join(xored)

def get_sympackets(dictstream,pkts):
    for pkt in pkts:
        if "ICMP" in pkt:
            break
        #print(pkt.summary())
        src = (str(pkt.src) + ":" + str(pkt.sport))
        dst = (str(pkt.dst) + ":" + str(pkt.dport))
        shash = hashlib.sha256(src.encode('utf-8')).hexdigest()
        dhash = hashlib.sha256(dst.encode('utf-8')).hexdigest()

        unique=xor_str(shash,dhash)

        if not unique in dictstream:
            dictstream[unique]=[]

        dictstream[unique].append(pkt)


def main():
    pkts_evil = rdpcap('evilstream2.pcap')
    pkts_noise = rdpcap('noise2.pcap')

    print("[+] DEBUG: done loading packets ")
    new_evil=[]
    new_noise=[]
    
    pkt_nr=0
    for pkt in pkts_evil:
        if not "IPv6" in pkt:
            if pkt[IP].src == "10.53.39.207":
                if pkt.haslayer(Raw):
                    try:
                        if binflag[pkt_nr] == '1':
                            pkt[IP].flags = 4
                        pkt_nr+=1
                    except IndexError:
                        pass


            del pkt[IP].chksum
            del pkt[TCP].chksum
            pkt[Ether].src="00:00:00:00:00:00"
            pkt[Ether].dst="00:00:00:00:00:00"
            new_evil.append(pkt)

    for pkt in pkts_noise:
        if "TCP" in pkt: del pkt[TCP].chksum
        if "UDP" in pkt: del pkt[UDP].chksum
        del pkt[IP].chksum
        pkt[Ether].src="00:00:00:00:00:00"
        pkt[Ether].dst="00:00:00:00:00:00"
        new_noise.append(pkt)
    
    pkts_evil=Null
    pkts_noise=Null

    wrpcap('a.pcap',new_evil)
    wrpcap('b.pcap',new_noise)
    new_evil=rdpcap('a.pcap')
    new_noise=rdpcap('b.pcap')

    evil={}
    noise={}
    get_sympackets(evil,new_evil)
    get_sympackets(noise,new_noise)
    
    rand=0
    for streams in range(len(noise)):

        lol = list(noise.keys())[streams]
        for pkt in noise[lol]:
            packet_time(pkt)
            covert.append(pkt)

        if random.randint(0,100) < 30:

            try:
                lol = list(evil.keys())[rand]
                for pkt in evil[lol]:
                    packet_time(pkt)
                    covert.append(pkt)
                rand=rand+1
            except IndexError:
                continue

    wrpcap("covert.pcap", covert)

if __name__ == '__main__':
    main()
