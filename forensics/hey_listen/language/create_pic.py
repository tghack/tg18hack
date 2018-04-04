#!/bin/bash
import os

#flag = "Fu Ra Gu No Na I Yo U Ha Kagi_1 Ko No Ze Ru Da No Pa Zu Ru Ha Ta No Shi I De Su Ne Kuten Kagi_2".split(" ")
#flag = "Fu Ra Gu No Na I Yo U Ha Kagi_1 Ko No Question No Question Question Ru Ha Ta No Shi I De Su Ne Kuten Kagi_2".split(" ")
#flag = "Fu Ra Gu No Na I Yo U Ha Kagi_1 Ko No Ze Ru Da No Pa Zu Ru Ha Ta No Shi I De Su Ne Kuten Kagi_2".split(" ")
#flag = "Fu Ra Gu No Na I Yo U Ha Kagi_1 Ko No Ze Ru Da No De N Se Tsu Pa Zu Ru Ga Su Ki De Su Ka Kagi_2 Kuten".split(" ")
flag = "Fu Ra Gu No Na I Yo U Ha Kagi_1 Ko No Ze Ru Da No De N Se Tsu Pa Zu Ru Ga Su Ki De Su Ka Kagi_2 Kuten".split(" ")

print flag

args = "".join([f + ".png " for f in flag])
print args

cmd = "convert " + args + "+append out.png"
print cmd

#for f in flag:
#    cmd = "convert {}.png -resize 110% {}_110.png".format(f, f)
#    print cmd
#    os.system(cmd)
#
#import sys
#sys.exit()
os.system(cmd)
