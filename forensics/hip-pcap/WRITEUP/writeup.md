# hip-pcap writeup

After downloading the pcap we open it in wireshark for initial analysis:
```
wireshark hip.pcap
```

This reveals a lot of normal looking traffic, mainly over TLS/HTTPS on port 443.

As its quick to get lost in the amount of traffic when doing network forensics, its a good idea to start by looking at different anomalies between the different hosts.

 
```
In Wireshark:
Toolbar > Statistics > Conversations
```

On the IPv4 tab we can see an overview of the IPv4 conversations that happened sorted by different options like packets, bytes, packets from a to b, packets from b to a etc. This is a good starting point as covert streams might stick out by having a large or small number of packets transferred. We can also play with the sorting options in the TCP tab and find that 136.243.2.42 sticks out by transferring almost the double amount of bytes from A to B.

If we apply this as a Wireshark filter and look closely on the "SSL" stream we can see that this is not an SSL/HTTPS stream, but a PNG exfil that uses port 443 (https) to hide. This is a known tactic used by malware and malicious actors to either use as a command-and-control channel or to download or upload data. A note is that wireshark acutally misinterpreted this as a SSL stream based on the destination port 443 even though this is not actaully SSL traffic.


```
.PNG
.
...
IHDR.............^..X....sRGB.........gAMA......a....	pHYs..........o.d....IDATx^....m....e..s.........Td..J.UU.$.[..W..^V......._..r[.VYb.HLo.9..C.?...y...@..P".@....c..=}w.s.=wy..M.._t.\]]../5...h..M".D.V.V.]^^....f...zU0..&..x..........t:..\..Fm94.yap:.].(.Vj....lv.t..@...2.yE.W+s...M...(.j.RR....V..uY.K.r.JH.S...?..(P.Z&S.I.R...I....._...J...9..W

```

If we save this TCP conversation as a "raw" stream in wireshark we get a PNG picture of a cap with the letter P on it.

When viewed by binwalk, a tool to examine binaries for embedded files and executable code, we find that the image also contains at the end an embedded gzip compressed archive.


```
$ binwalk p-cap.png 

DECIMAL       HEXADECIMAL     DESCRIPTION
--------------------------------------------------------------------------------
0             0x0             PNG image, 466 x 262, 8-bit/color RGB, non-interlaced
91            0x5B            Zlib compressed data, compressed
101298        0x18BB2         gzip compressed data, from Unix, last modified: 2018-01-27 13:26:50

```


This can be extracted by using binwalk and outputs the files in a new directory '_p-cap.png.extracted'

```
$ binwalk -e p-cap.png
```


Then by unpacking the 18BB2 gzip archive using tar we find what has been hidden inside:


```
$ tar xvf 18BB2
meta
$ cat meta
544731387b502d4341502d30522d31542d4431444e27542d48345050334e217d
```

This looks like hex encoded ASCII that we can easily decode to finally get the flag:


```
xxd -r -p meta
TG18{P-CAP-0R-1T-D1DN'T-H4PP3N!}
```

GGWP
