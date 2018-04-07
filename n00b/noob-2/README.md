# Web Security for Beginners - part 2
**Points: 30**

**Author: maritio_o**

The previous module presented tasks about 
how everyone can obtain information such as the source code of the frontend, 
and monitor a web page's incoming and outgoing network communication.

In this module, we will try to find more hidden information by digging a little further, check out the two new web pages:

* [noob2-1.tghack.no](http://noob2-1.tghack.no)
* [noob2-2.tghack.no](http://noob2-2.tghack.no)

The tasks are:
1. "Biip Boop. Exterminate. Exterminate. Bip. Bop." (noob2-1)
2. I just love flags. I didn't know which flag to use, so I kept a whole bunch of them! (noob2-1)
3. Knowledge of the file system layout can be useful. (noob2-2)

### Noob2-1 File Structure:
```
src
├── flag.txt
├── index.html
├── pictures
│   └── flags
│       ├── communist_flag.jpg
│       ├── flag.png
│       ├── japan_herro_kitty_flag.jpg
│       ├── mother_of_flags.jpg
│       ├── norway_flag.png
│       └── pride_flag.jpg
├── robots.txt
├── style.css
└── tghack.png
```


### Noob2-2 File Structure:
```
src
├── pictures
│   └── flags
│       ├── communist_flag.jpg
│       ├── flag.png
│       ├── japan_herro_kitty_flag.jpg
│       ├── mother_of_flags.jpg
│       ├── norway_flag.png
│       └── pride_flag.jpg
├── secret
│   └── flag.txt
├── server.py
└── static
    ├── index.html
    ├── style.css
    └── tghack.png
```


> Do you need help to find the flags? 
> 
> Read Module 2 of the [tutorial](http://tghack.no/tutorial.html), 
> or stop by the TGHack area in the Creative Lounge to get help!
