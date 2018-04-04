# P-IS-FOR-PCAP writeup

We start by opening the PCAP in wireshark, i.e like this: 

```
wireshark p-is-for.pcap
```

You can also open the pcap file with the menu by clicking 'File>Open' and selecting the correct file, here "p-is-for.pcap"
If you prefer other tools, you can use tcpdump, tshark, scapy or other packet analyzers.

Looking around we can observe multiple streams to and from "192.168.220.15".

We continue to inspect the http streams from "192.168.220.15" by applying a wireshark filter like 'http && ip.addr==192.168.220.15' and observe multiple request towards the url '/flag/' and some containing pars of the flag.
We could also use a more general display filter to find the start of the flag like this: 'ip contains "TG18"' and figure out the rest that way.


To get/view the flag we can use wiresharks "follow http stream" on each of the streams to append the flag together by rightclicking on the packet, select 'follow>HTTP Stream', or use tshark with a filter to output just the http media like this:

```
tshark -r p-is-for.pcap -Y 'http && ip.src==192.168.240.2' -T fields -e http.file_data
```

Which gives us this output:

```
THEFLAG

THATYOUARESEEKING

ISHERE

TG18

{
EASY

PCAP

EASY

LIFE
}

```

By removing the extra words and concatinating the strings, we get the final flag:
```
TG18{EASYPCAPEASYLIFE}
```
