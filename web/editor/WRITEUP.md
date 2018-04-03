# Writeup Editor
If you add an image in the editor and open the inspect window in your
browser you can see that the URL to the image has changed to `/proxy/HEX_STRING`.
If you decode the hex string you can see that this is the URL to the image you added.

Since the server proxies an URL from the user it is possible to connect to hosts
that are not open to the internet. If you run the binary and run `lsof -i -P`
you will get something like this

```
server    7714 roypur    3u  IPv4 151361      0t0  TCP localhost:2202 (LISTEN)
server    7714 roypur    5u  IPv4 161849      0t0  TCP localhost:2201 (LISTEN)
server    7714 roypur    6u  IPv6 157149      0t0  TCP *:2200 (LISTEN)
```

We can see that the server listens on port 2201 and 2202 on localhost.
If we try to connect to localhost on port 2201 by inserting an
image with url `http://127.0.0.1:2201` we get a broken image.
If we open the image in a new tab we get a file listing with the file `flag.txt`
If we click the link it opens [https://editor.tghack.no/flag.txt](https://editor.tghack.no/flag.txt)
and we get a `404 Not Found` error. This is because it is not going through the proxy anymore.
If you add `http://127.0.0.1:2201/flag.txt` as an image and open the image in a new tab we
get the contents of the file which is the flag.

`TG18{be_ware_of_proxies}`
