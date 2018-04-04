#!/usr/bin/python3

from scapy.all import *
import hashlib
import random
from random import uniform

#globs
pkttime=1500000000
easy=[]

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


def packet_time(pkt):
    global pkttime
    pkt.time=pkttime + uniform(0.001, 0.5)
    pkttime=pkt.time
    return

if __name__ == "__main__":

    pkts_flag = rdpcap('local-flag.pcap')
    pkts_noise = rdpcap('noise.pcap')

    new_flag=[]
    new_noise=[]

    for pkt in pkts_flag:
        if not "IPv6" in pkt:
            if pkt.sport == 4443:
                pkt[IP].src = "192.168.240.2"
                pkt[IP].dst = "192.168.220.15"
                pkt.sport = 80
            elif pkt.dport == 4443:
                pkt[IP].dst = "192.168.240.2"
                pkt[IP].src = "192.168.220.15"
                pkt.dport = 80

            del pkt[IP].chksum
            del pkt[TCP].chksum
            pkt[Ether].src="00:00:00:00:00:00"
            pkt[Ether].dst="00:00:00:00:00:00"
            new_flag.append(pkt)

    for pkt in pkts_noise:
        if ("TCP" in pkt) or ("UDP" in pkt):
            if pkt.sport > pkt.dport:
                pkt[IP].src = "192.168.220.15"
            else:
                pkt[IP].dst = "192.168.220.15"

            if "TCP" in pkt: del pkt[TCP].chksum
            if "UDP" in pkt: del pkt[UDP].chksum
            del pkt[IP].chksum


        pkt[Ether].src="00:00:00:00:00:00"
        pkt[Ether].dst="00:00:00:00:00:00"
        new_noise.append(pkt)

    wrpcap('a.pcap',new_flag)
    wrpcap('b.pcap',new_noise)
    new_flag=rdpcap('a.pcap')
    new_noise=rdpcap('b.pcap')

    flag={}
    noise={}
    get_sympackets(flag,new_flag)
    get_sympackets(noise,new_noise)

    rand=0
    for streams in range(len(noise)):

        lol = list(noise.keys())[streams]
        for pkt in noise[lol]:
            packet_time(pkt)
            easy.append(pkt)

        if random.randint(0,100) < 10:

            try:
                lol = list(flag.keys())[rand]
                for pkt in flag[lol]:
                    packet_time(pkt)
                    easy.append(pkt)
                rand=rand+1
            except IndexError:
                continue
    wrpcap('p-is-for.pcap',easy)

