# Writeup [universal-serial-bus](https://github.com/PewZ/tg18hack/tree/universal-serial-bus/forensic/universal-serial-bus)

In this task we are given a pcapng file containing USB data.

If we open this in wireshark, tcpdump, tshark or $yourfavoritepacketanalyzer we can see a lot of USB interrupts indicating a HID device, possibly a keyboard or a mouse, followed by a new USB Mass Storage device sending and receiving data using SCSI.

There are multiple ways of solving this task and in this writeup I'm using [Scapy](https://github.com/secdev/scapy).

Scapy is one of my favorite tools for pcap analysis and network related fun. It is also able to handle PCAPS that contain USB data by processing the packets as 'raw', which means that we can use it to find out what the attacker typed on the keyboard by automating translation from USB HID Keyboard codes to a more human readable format. I modified a script I found [here](https://raw.githubusercontent.com/dbaser/CTF-Write-ups/master/picoCTF-2017/for80-just_keyp_trying/usbkeymap2.py) that outputs the keystrokes from the pcap: [solve-keystream.py](https://github.com/PewZ/tg18hack/blob/universal-serial-bus/forensic/universal-serial-bus/writeup/solve-keystream.py).

As we have not filtered the PCAP for only the usb stream we get some misplaced data from this script. I have also not included a way to switch character based on modifiers(shift,ctrl,alt etc), but we still manage to get the key point of what commands the attacker typed:

```
LS /
LS /SECRRET-STUFF
CCAT /SECRET-STUFF/STUFF
KC;}KK32722O5W1LDLSBLK
MOUNT /DEV/SDB1 /MNT/USB1/
LDLS /MNT/USB1/
LS /MNT/USB1/
LS /MNT/USB1/LOL
CAT /SECRRET-STUFF/STUFF  OPENSSL EENC -BASE664 -E -AES-256-CBC -NNOSSALT  -PASS PASS;T0PK3K > /MNT/USB1/STUFF
UMOUNT /MNT/USB1
4333AA3AADDDEE3EEFF3FFGGG
```

By manually cleaning out duplicate characters and manually changing modifiers, we can see that the attacker has mounted a USB drive, cat'ed out the `/secret_stuff/stuff` file and outputed it to the usb drive encrypted using the following openssl command:

```
cat /secret_stuff/stuff | openssl enc -base64 -e -aes-256-cbc -nosalt -pass pass:t0pk3k > /mnt/usb1/stuff"
```

Now if we look for the "stuff" file or a base64 string using `strings` on the pcap we see the encrypted content of the stuff file:


```
$ strings usb.pcapng

...
...
WLWL
LOLASD      
WLWL
3OR         
XLXL
AFDSJ       
WLWL
SDFDFQ      
WLWL
STUFF       
\L\L
B           
WLWL
P[A1
1RED        
XLXL
USBS
USBC
BnyFsr42DMNOihPWButlEKh0KtOoKONN3qO+7qiZfzx+tWBVsL+zXTf0VkaZWXff
USBS
USBC
USBS
...
...
```


If we take that base64 string and use the same openssl command as the attacker, switching encryption for decryption with -d, we get the content of the encrypted file:

```
echo "BnyFsr42DMNOihPWButlEKh0KtOoKONN3qO+7qiZfzx+tWBVsL+zXTf0VkaZWXff" | openssl enc -d -base64 -aes-256-cbc -nosalt -pass pass:t0pk3k
TG18{USB-DATA-IN-MY-PCAP-T0TAL-BUZZK1LL}
```
