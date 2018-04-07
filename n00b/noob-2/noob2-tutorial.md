# TGHack: Web Security Tutorial - page 2

## Module 2: File Paths in Web Pages
In this module, there are two web pages to hack:

* [noob2-1.tghack.no](http://noob2-1.tghack.no)
* [noob2-2.tghack.no](http://noob2-2.tghack.no)

The tasks are:
1. "Biip Boop. Exterminate. Exterminate. Bip. Bop." (noob2-1)
2. I just love flags. I didn't know which flag to use, so I kept a whole bunch of them! (noob2-1)
3. Did you know that web pages also use the file structure? (noob2-2)

## Introductional Information
Web pages are often hosted by running a file on a server having a file system. 
The file system consists of directories and files, just as on your computer. 
An example is the `File Explorer`, or your terminal, 
where you have lots of files and directories. 
This module is about path traversal within web pages.

As mentioned in the previous module, the frontend code is open for everyone.
Not only the code is open for everyone, but also all the other files, such as pictures or audio files. 
In order for the frontend to work, all the sources must be made available for everyone.
To make the sources available, files or directories are made public. 
It is important to make sure that only the bare minimum of files are open to the public — we do not want users to access sensitive files.

It is important to mention that many web development systems nowadays 
have built-in security measures against 
allowing users to traverse through the file system from a web page. 
Therefore, this module might be the least realistic one in the n00b category.

## Web Page Structure
In this module, the frontend for both web pages have common folder structures.
See the snippet below for an illustration of the folder structure.
Both web pages have a main folder called `src`.
We usually call this the `source folder`, and will continue calling it so in this tutorial.
The source folder contains all the files needed to run the web page. 
In addition, we have a folder called `pictures`, 
which contains another folder called `flags` with lots of pictures of, you guessed it, flags.

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

As mentioned, there are security measurements against directory traversal in many 
web development tools. 
In order to make the web pages vulnerable, 
we used two tools to host the web pages.

Noob2-1 is hosted using `nginx`, which is a web server.
Web server has a broad definition, and can be any piece of software or hardware dedicated to serving web content.
`nginx` can also be used as a [load balancer](https://en.wikipedia.org/wiki/Load_balancing_(computing)) and a [reverse proxy](https://en.wikipedia.org/wiki/Reverse_proxy) (more on this in the next section).
We have configured `nginx` to read files from the `/var/www/noob2/src` directory.
It will always serve `index.html` if not given any other path.

Noob2-2 also uses `nginx`, but this time as a reverse proxy.
Which means that `nginx` will communicate with another service on the server to fetch files, etc.
All the requests go through `nginx`, but all the work with fetching the correct content and other server logic is handled by a `Flask` application.
This can be used to have `nginx` serve content from several web server applications on one machine.
`Flask` is a micro web framework written in Python. It enables us to write web applications quickly in Python.
The directory structure looks similar to the one used with `nginx`:
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

The main difference is the `static` folder, where `index.html`, `style.css`, and `tghack.png` are located. This time there is no `robots.txt` either.

All the source code can be found in `server.py`.

### The Robots Exclusion Protocol
The first task has a big hint in the description. The text reminds you of a robot, doesn't it? 
By googling a bit, you may learn that it is normal to keep a file called `robots.txt` on your web page. 
The `robots.txt` file gives instructions about the web page to web robots. 
This file is always placed at the top-level directory of the web server. 
The usage of the file is called _The Robots Exclusion Protocol_. 

So, what happens is that a web robot wants to visit a web page, such as `https://noob2-1.tghack.no/welcome.html`. 
Before it visits the page, it checks the `robots.txt` file at `https://noob2-1.tghack.no/robots.txt`.
The robot will not visit any of the pages listed on that page. 

**It is important to note that not all robots obey these instructions.
Malicious robots such as malware robots that scan for vulnerabilities or
email harvesters used by spammers might use these to find interesting information**

**In addition, it is important to notice that all the information in robots.txt is publicly available. 
Anyone can see the sections that the server hosting the web page don't want the robots to read.**

Now, let's try to find the flag!

```
Task 1: "Biip Boop. Exterminate. Exterminate. Bip. Bop." (noob2-1)
```

> Did you find the flag? Here is the solution:
>
> First, let's have a look at the `robots.txt` file at [https://noob2-1.tghack.no/robots.txt](https://noob2-1.tghack.no/robots.txt). 
> We see the only disallowed page is the `flag` page. 
> Checkout the page, and get the flag.

The lesson is to keep as much information as possible to yourself, 
not publicly on the web. 

### Available Files in the Source Code


As you may have noticed, the front page contains a nice picture of the Norwegian flag. 
Pictures are often put into a certain folder with other pictures. 
In some vulnerable web pages, it is possible to go to that folder by
entering the path into the URL. 
Finding the path is as simple as inspecting the page,
and looking at the HTML code where the image is placed. 

Try it out, and find the flag!

```
Task 2: I just love flags. I didn't know which flag to use, so I kept a whole bunch of them! (noob2-1)
```

> Did you find the flag? Here is the solution:
>
> By looking at the source code, we may see the path of the file within the source code. 
> The image path in the HTML is `pictures/flags/norway_flag.png`.
> Remove the image file from the URL and paste into the URL.
> One of the image files contains a picture of the flag.

The lesson here is that it is important to be careful with the 
files that are actually added to the frontend. 

### Traversing Out of the Source Folder
Thus far, we have looked at files in the intended file path.
Meaning that the files are all within the source directory of the web server.
`/var/www/noob2/src` for the first two tasks, for example.
Is it possible to traverse out of the source folder,
and into other directories within the web server?
Yes, it is, and it is called `Local File Inclusion (LFI)`!

Well, it is not always possible... In this task, the LFI vulnerability is there
because of bad coding choices.
This may also be a good opportunity to mention that this is a good example
of why reinventing the wheel is not always appropriate. 
Depending on the situation, there might exist code or libraries that 
developers should use for their programs. 
Custom solutions may do more harm than good,
but choose the libraries with care!

Let's go through how you may traverse out of the source folder and into the 
file system in a web page hosted like this.

If we visit noob2-2.tghack.no, we get a page that looks just like the first one.
However, if we take a look at the source code, we can see that the pictures are loaded a bit differently.

```
<img src="pictures/flags/?name=norway_flag.png" alt="Norwegian Flag" class="flag_image">
```

Notice the part after `/flags/`: `?name=norway_flag.png`.
The part after the question mark is called a [query string](https://en.wikipedia.org/wiki/Query_string).
Query strings are used for URL components that do not fit into the conventional hierarchical structure.
In this case, it is used to specify the file name of a flag to load.
To load another flag, we can visit the URL https://noob2-2.tghack.no/pictures/flags/?name=pride_flag.jpg for example.
The server code is flawed, however, as it allows us to fully control the file name.
This enables us to traverse **outside** of the intended directory by using `../` in the path.
`..` means the *parent directory*, or the directory *above* the one we are currently in.
We know that the flags are loaded from `pictures/flags/`, so we need to specify `../../` to traverse outside the `pictures` folder.
From here we can access the server code, for example.
To do so, visit `https://noob2-2.tghack.no/pictures/flags/?name=../../server.py`.
If you are curious, you can check out the beautiful source code.

Alright, time to find the flag:

```
Task 3: Did you know that web pages also use the file structure? (noob2-2)
```

> Did you find the flag? Here is the solution:
>
> We are already very close to the solution as we know how to get the contents of `server.py`.
> The next step is to read the flag from `secret/flag.txt`.
> The `secret` folder is in the same folder as `server.py`, so we add `/secret/flag.txt` to the previous query to get the flag.
> Like this: `https://noob2-2.tghack.no/pictures/flags/?name=../../secret/flag.txt`
