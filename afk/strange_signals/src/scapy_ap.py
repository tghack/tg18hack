#!/usr/bin/python3
from scapy.all import *
from os import path
import subprocess
import time

iface=""
SSID="TG18{wir3less-3aster-b34c0ns?}"
#SSID="TG18{TESTFLAG}"

rngmac=RandMAC()

#setup
dot11=Dot11(addr1="ff:ff:ff:ff:ff:ff", addr2=rngmac, addr3=rngmac)
beacon= Dot11Beacon(cap='ESS+privacy')
essid=Dot11Elt(ID='SSID',info=SSID, len=len(SSID))
rsn=Dot11Elt(ID='RSNinfo', info=(
'\x01\x00'                 #RSN Version 1
'\x00\x0f\xac\x02'         #Group Cipher Suite : 00-0f-ac TKIP
'\x02\x00'                 #2 Pairwise Cipher Suites (next two lines)
'\x00\x0f\xac\x04'         #AES Cipher
'\x00\x0f\xac\x02'         #TKIP Cipher
'\x01\x00'                 #1 Authentication Key Managment Suite (line below)
'\x00\x0f\xac\x02'         #Pre-Shared Key
'\x00\x00'))               #RSN Capabilities

wifi_frame=RadioTap(present=0)/dot11/beacon/essid/rsn

def scheduleloop(schedule):
    for stime in schedule:
        if (int(time.time()) == stime):
            return True


def calcsched():
    schedlist=[]
    start=1522098000
    for i in range(21):
        if not schedlist:
            schedlist.append(start + (start % 3)+20000)   
        else:
            schedlist.append(schedlist[i-1] + (schedlist[i-1] % 3)+20000 )

    #lol = calcsched()
    #for a in lol:
    #    print("epoch: " + str(a) + "human: " + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(a)))
    return schedlist


def setup_wificard():
    card=""
    test = os.listdir('/sys/class/net/')
    for lol in test:
        if 'mon' in lol:
            print("already a monitor interface")
            break
    
        addr = subprocess.check_output(['/usr/bin/cat', '/sys/class/net/'+lol+'/address'])
        print("addr:" + str(addr))
        #input wifi mac here
        if "00:00:00:00:00:00" in str(addr):
            card=lol
            print("found card: " + lol)
    
            if subprocess.check_output(['/usr/bin/airmon-ng', 'start', card]):
                card = card + "mon"
                print("Card successfully in monitor mode: " + card )
                return card
    
 #__import__("IPython").embed()

def fake_broadcast(iface):
    
    broadcast_time = time.time() + 60 * 3
    while time.time() < broadcast_time:
        sendp(wifi_frame, iface=iface, inter=0.01, loop=0)


if __name__ == '__main__':
    iface = setup_wificard()
    #iface = 'wlp0s20u3mon'

    schedule=calcsched()
    #schedule=[1522014200]
    while True:
        if scheduleloop(schedule):
            print("Starting broadcast")
            fake_broadcast(iface)
            print("Stopped broadcasting")
        time.sleep(1)
