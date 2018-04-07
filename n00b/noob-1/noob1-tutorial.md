# TGHack: Web Security Tutorial - page 1

## Module 1: Information in the Code

The web page to hack is:
```
noob1.tghack.no
```

The tasks are:
1. Find the comment with too much information.
2. Can you find another comment that might lead to something secret? 
3. "C is for cookie, that's good enough for me"
4. Have you heard of tokens before? Maybe you would like to be introduced to Jason, he seems to know a lot about tokens. 

### Introductory Information
Many developers of web pages write comments in their code to remind themselves of things that need changes or fixes, 
or just to remember how something within the code works.
Often certain information is required to make web pages, 
and there are also a handful of options to store information within a web page. 
The information in the frontend should always be kept to a bare minimum.

All the tasks in this module are about information in the frontend code. 
Frontend code is the code that all the users of a web page can see, 
so the goal of these tasks is to find information that the developers 
should not have added when deploying their web page. 

### Source Code Information
Alright, let's try to find some information from the web page. 
However, I would like to explain a little about the frontend code first.
Frontend code usually contains three different types of files:

* HTML (.html): Where the setup of a page is programmed.
The page is set up with components, such as text fields, input fields and buttons.
* JavaScript (.js): Scripting to interact with the HTML code, and do lots of other logic.
* CSS (.css/.scss): Styling of elements from the HTML code, such as placement, coloring, and sizes.

There are several ways to find the frontend source code:

* Right-click on the page, and choose the `View page source` option. 
However, this will only open the HTML code for the specific page you right-clicked on. 
* Right-clicking in a web page and choose the `Inspect` option, and open the `Sources` tab. 
In here, we find all the files used in the current page. 

### Solving Some Tasks
Ah yeah, now we can try to find some information from the page. 
Go through all the source files, and then see if you can find the flags. 
Remember that the flags are in the following format: `TG18{some random text here}`.

> Did you find the flags? Here is the solution:
> 1. There is a plain text flag logged into the console when opening the page. 
> The flag may be found either in the `Console` tab in the `Inspect` window, 
> or by looking in the `script.js` JavaScript file, where the log is coded and you may find the flag.
> 2. A Comment in the HTML file says `Remember to fix redirect to /secretpage/`. 
> That is a hint that there might be another page in our domain.
> By adding /secretpage to the URL, like this: `noob1.tghack.no/secretpage/`,
> we enter a new page containing the flag.


### Network Communication Information 
The `Inspect` window is a very important page for web development and web security. 
You will probably use it a lot in CTFs!

The page contains a lot more information about a web page than the just the source code. 
For instance, a web page often interacts with a backend. 
The backend is the part of a system the users do not see, 
and will often contain a lot of important information. 
We will explain more about the backend in another module. 
What's important for this module is that you know how the communication with the backend looks like from the frontend. 

So, lets open the `Network` tab in the `Inspect` window.
In the `Network` tab, we see all the incoming and outgoing communication to the frontend code.
The frontend usually asks the backend for some piece of information, 
within something that is called an `HTTP request`. 
When the backend answers, it sends something called a `response`.
Requests and responses are sent through the HTTP or HTTPS protocol. 
To keep it simple, think of the HTTP/HTTPS protocols as messengers that carries 
the message from a page to another, or from a system to another. 
Each message has something called `headers`. 
The headers can contain lots of options to configure things like the type of data that is sent, 
and what type of security we want when we send the message. 
The headers are often used to hold something called `tokens` or `cookies`,
which is what we need to know about in this module. 

Tokens and cookies often hold information about a user,
in order to remember information until next time the user enters the web page.
For instance, have you noticed that you don't have to log in to most web pages
after you have logged in once? 
But then you go to the web page on another machine, and you have to log in there?
Most pages use cookies or tokens to hold onto this information. 
The cookies don't have a special structure, 
and may store encoded or plain text data. 
A specific type of token which is popular now is the JSON Web Token (JWT).
The JWT's have a special format with a header, a payload, and a signature. 
All of these parts are separated by a dot, like in the following JWT token:

```
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6Im1hcml0aW9fbyIsImFkbWluIjoiSGVsbCBZZWFoISJ9.e-Ohmw-w_zk-fIadhC_C_UTRZeJx0FLlnEhLRu9jBIM
```

You can try to decode the JWT above with [this JWT decoder](https://jwt.io), or find any decoder by googling.

### Solving Some More Tasks!
Let's try to find the cookie and/or token in the header of a response 
or request in the `Inspect` page. 
As we entered the `network` tab after the web page was loaded, 
we might need to refresh the page. 
When the page is refreshed, some requests or responses should turn up. 
Click on any of the responses or requests, and another small window will appear.
The new window has a few tabs, we are interested in the `Headers` tab.
Click on it, and all the headers for that specific response/request will appear. 
Not all requests or responses sends the cookie or token, 
so now you have to look for them. 

> Did you find the flags? Here is the solution:

> 3. Any of the requests from our web page, for instance when loading the page, 
> fetching the background picture, or our `script.js` file, 
> contain a cookie with the flag.

> 4. The last flag is within a JSON Web Token (JWT). 
> The JWTs encode the header and payload, meaning that the flag is not in plain text. 
> We also do not find any JWT until we have logged into the web page.
> The JWT may be found as an `X-Access-Token` header, and looks like this:
> ```
> eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJtYXJpdGlvX28iLCJpYXQiOjE1MTg5NDQ2Mzk3NzMsImV4cCI6MTUxOTM3NjYzOSwiaXNzIjoibWFyaXRpb19vIiwiYXVkIjoiVEcxOHthbnkwbmVfdXNpbmdfdGhpc190b2tlbl9jYW5fcHJldGVuZF90b19iZV95MHVfMG5fdGhpc193ZWJzaXRlfSJ9.b8oavuy98jJ82OB6iVCTCEHePvo92SsrAM8i3vwRpts
> ```
> By using a JWT decoder, we get a payload which contains the flag.
