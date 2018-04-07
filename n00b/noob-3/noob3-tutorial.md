# TGHack: Web Security Tutorial - page 3

## Module 3: Getting the Admin Rights
In this module, there are four web pages to hack:

1. [noob3-1.tghack.no](http://noob3-1.tghack.no)
2. [noob3-2.tghack.no/api](http://noob3-2.tghack.no/api)
3. [noob3-3.tghack.no](http://noob3-3.tghack.no)
4. [noob3-4.tghack.no](http://noob3-4.tghack.no)

In all of these web pages, the goal is to either get to the admin page, 
or get the admin rights to information.

## Introductory Information
Malicious hackers often aim for administrator rights.
Administrators, or admins, are most likely to have more 
rights and privileges than the normal users. 
Why hack your opponent in TGHack for her flags,
when you can hack the admin user and get all the flags?
However, hacking normal users might be necessary to get to the admin users.

There are several ways of getting into another person's account,
or get to their data. 
In this module, we demonstrate four possible methods for doing so. 

## SQL Injections
Web pages must have some way of storing all their users names, 
passwords, and other information.
Databases are mostly used to store information, 
and is therefore a valuable target for hackers.
In order to understand some of the tasks in this module,
we must understand a little about SQL. 

### How SQL Works
Many databases use a language called **SQL**, 
which we use in the tasks in this module. 
SQL is short for **Structured Query Language**, 
and uses **SQL queries** to create or manage databases.
It is possible to add, remove, or change data in a database
using SQL queries, among other things.
Each database has at least one **table**, and each table contain **fields**. 
There are lots of other stuff to configure, such as attribute types and
the size of each attribute.
but you don't have to worry about that!

As an example, let's say we have a database called `tg18hack`.
The database stores all users in a table called `users`.
The table has a field called `username`, and another one called `password`.
This is an output of the `users` table:

```
+----------+-------------+------+-----+---------+----------------+
| Field    | Type        | Null | Key | Default | Extra          |
+----------+-------------+------+-----+---------+----------------+
| id       | int(6)      | NO   | PRI | NULL    | auto_increment |
| username | varchar(20) | NO   |     | NULL    |                |
| password | varchar(50) | NO   |     | NULL    |                |
+----------+-------------+------+-----+---------+----------------+
```

Having three users in the database, the table looks like this:
```
+----+-----------+-----------------+----------------------------------+------------+
| id | username  | email           | password                         | date       |
+----+-----------+-----------------+----------------------------------+------------+
|  1 | admin     | admin@tghack.no | 9cdfb439c7876e703e307864c9167a15 | 2018-03-28 |
|  2 | maritio_o | marit@gmail.com | 0cb9b0c2526ce4b3d475307b75e9d36d | 2018-03-29 |
|  3 | roypur    | roy@pur.com     | 5e591308ea5dbc5062197b586d0803fa | 2018-03-29 |
+----+-----------+-----------------+----------------------------------+------------+
```

### Hacking Web Pages using SQL
Web pages that store user information using SQL may use it to 
handle authorization when users log in to their pages.
The web page sends the login credentials (username and password) to the backend.
The backend uses SQL to check whether the user exists in the database, 
and verifies that the given password is the same as the stored password in the database.
The stored password is the one you always write when 
registering your user the first time on a system. 

This procedure is the one that is exploited (hacked) in an `SQL injection`.
In short, SQL injection can be described as _exploiting vulnerable systems using SQL_.

Now, let's open the first task, [noob3-1.tghack.no](http://noob3-1.tghack.no).
This nice login page contain two input fields, 
one for your username and one for your password. 
When these credentials are sent to the backend, 
the backend uses the following SQL query to check if the password is right,
where `'username'` and `'password'` are the ones that the user entered to log in:
```
SELECT * from users WHERE username = 'username' and password = 'password';
```

In this query, a `SELECT` is used to tell the system we want to fetch data, 
`*` is used to specify that all matching data should be returned,
`from users` means that we should check the `users` table, 
and at last we specify that the returned data must have a username field 
containing the given username, and a password field containing the given password. 

To trick the web page's login procedure to give us access, 
we must use input that will change the SQL query to return some data. 
If the SQL query returns data, it means that it found a match in the database. 
Depending on the security in a system, this might allow you to log into a page
without a username. 

But what input can manipulate the SQL query to do something like that?
The one method almost all hackers know about is entering `m' OR '1'='1'; --`.
This piece of text contains characters that have special meaning in SQL, such as:

* `'` ends the input field.
* `OR` is a binary operator. Read about binary operators [here](https://www.techopedia.com/definition/23953/binary-operator).  
* `'1'='1'` is a dummy input that always is true.
* `;` ends the query.
* `--` comments out everything that follows.

Shortly explained, the snippet ends the input field to write more SQL, 
makes sure that the query returns true, 
and removes anything coming after it so that nothing ruins our new query 
and have more control.
Given that we enter **maritio_o** into the username input field, 
and the snippet in the password input field,
the new query ends up like this:
```
SELECT * from users WHERE username = 'maritio_o' and password = 'm' OR '1'='1'; --';
```

Sometimes the password is hashed before it is used in the SQL query, making the
password field useless for SQL injection. We can, however, use the username
field instead. If we want to log in as `maritio_o`, we can terminate the SQL
query to trick the backend into selecting this user by submitting the username
`maritio_o' --`. This will turn the query into the following:
```
SELECT * from users WHERE username = 'maritio_o' --' and password = '';
```
Recall that everything after `--` is ignored, so the query above is equal to:
```
SELECT * from users WHERE username = 'maritio_o';
```

Now that you know the basics of SQL injections, 
it is time to try it out on the web page!
Go to [noob3-1.tghack.no](http://noob3-1.tghack.no),
and inject some SQL!

> Did you find the flags? Here is the solution:
>
> The first task can be solved by writing `admin'--` in the `username` input
> field. The backend replies with the flag!

### SQL Injection Directly to the API
Obs! Doing SQL injections (or any other attack regarding input)
is not only possible from the input fields in the web pages.
Every other way to insert input may be used for SQL injections, 
such as tools that communicate directly with an API.
An API is code that receives messages, which are called **requests**,
and sends messages which are called **responses** back på the requester.

> An example of a tool that communicates directly with the API is `Postman`.
> To solve some of the tasks in this module, you will need such a program.
> Download `postman` [here](https://www.getpostman.com/).

The API often communicates to a database.
It is the API's task (or the developer of the API) to make sure 
to validate ALL the input it gets. 
If there is no input validation or the input validation is bad,
then users may get access to information they are not meant to find.
Every little piece of input should be checked before sending it any further into the system.
You will experience input validation and how to bypass it in the next module.

An example of an API call looks like this: 

![Postman API call example](postman_apicall_example.PNG)

In the above example, we see the following:
* An URL for the API: `http://noob3-2.tghack.no/api/login`. In this URL, the beginning `noob3-2.tghack.no/api` is the base url. The following part is what describes which API call we are doing. In this case, we are doing a `/login` request.
* By choosing `POST`, we define what type of request we are doing. Common request types are `POST, GET, PATCH, DELETE`, and there are many more.
* The input parameters are in the `body`. In this case, we use a body in the JSON format. 

When pressing the `Send` button, you will get some response in return. Maybe you get the flag? Let's try it out!

> Did you find the flags? Here is the solution:
>
> This task has the same backend as the previous, 
> so entering the same inputs as in the previous task
> makes the API respond with the flag.


## URL Input Parameters
Until now, we have been through two methods to insert input to a web page. 
There are more. In this section, we present input through the URL in the browser.
You may have noticed that you can change some URLs slighty to get to the page you want.
For instance, you may remove parts of the URL, or change a word in the URL.
This is because the web servers are setup to process the URLs. 
To exploit a web page through the URL, you should understand a few things about it.

Web pages redirect users to certain pages by processing the URLs. 
For instance, we may take a look at the navigation bar at our web page,
and see that the web page contain the pages `FAQ`, `About Us`, `Tips` and `Home`.
Let's open the tghack webpage. Then the URL is `http://tghack.no/index.html`.
That page is the same as wehn we click the "Home" button.
When clicking the other buttons, it is noticable that the only thing that changes in the URL
is the name of the HTML file that it uses to show the page. 
Instead of pressing the buttons, it is possible to change the HTML file name in the URL, 
which makes the web server open your page. 
Sometimes you may find pages that are not supposed to be shown, 
but are not removed from the web server. 
An example of this may be found in Module 1. 

In addition to opening pages through the URL, 
some pages have input parameters. 
These parameters may be used to open the right page for users.
For instance, users may have an ID. 
Imagine three users on a page, **admin**, **maritio_o**, and **roypur***.
These users have the following IDs: **000001**, **001337** and **001338**
A webpage use the following URL to open roypur's profile:
`https://some.random.webpage/profile?id=001338`.
If the security is not sufficient on the web page,
roypur could change the ID on the URL to open maritio_o's
or admin's profile, like in the picture below. 

![URL input example](url_input_example.PNG)

I think you know enough to get admin rights to the next task.
Take a look at [noob3-3.tghack.no](http://noob3-3.tghack.no),
and try to find the flag. Come by the Creative Lounge if you need help!

> Did you find the flags? Here is the solution:
>
> Change the URl from `htts://noob3-3.tghack.no/home?user=brukernavn`
> to `http://noob3-3.tghack.no/home?user=admin`, and you will get the flag in response.


## Using Other's Sessions
The last task in module 3 is similar to the previous task, 
but a little more advanced. 
This time, we present session hijacking. 
The hijacking itself is a little too complex, 
so we provide the snippet holding the session and then you may learn how to use it to get to the admin page.

Most web pages today handles session so that the users don't have to log in everytime, 
does not get logged out or so that the users don't have to redo something. 
As an example, you don't have to log into facebook every day, right?

Sessions are often handled by storing information in the headers that are sent through
HTTP, such as cookies or tokens. 
You got introduced to JWT in module 1, which may be used to handle automatic login.
When using JWT for automatic login, the token must be stored somewhere on the device that uses the application. 
The next time the user opens the application, the application reads the token and either 
redirects the person to a login page or into the application.
If the token is valid, the user is redirected to the application. 
However, what if some malicious user gets the token and uses it on his or hers own device?
Yup, you are thinking right.. That malicious person also gets logged straight into 
the application, and the application thinks he is the owner of the token.

There are several ways to protect the application from such attacks, such as:
* Making the token valid for only a short amount of time.
* Making it possible for the user to invalidate the token. This is usually done by "Manage your devices"-pages. For instance, you may log out of HBO on specific devices through the user profile. This is a nice feature if you suspect being hacked, or if a device is lost. 

Tokens ma be used by the API to make sure a user is allowed to do the HTTP request she or he is doing,
or to identify users. In other words, a token may hold the information about the users authorization to the API.
There are several ways to send a token to the API, but it is usual to send it as an HTTP header. 
You already looked at the HTTP headers to an HTTP request in Module 1, 
when you saw the JWT the first time in the `Inspect` tab.
Instead of reading the header, you must set the header this time. 
You may use Postman to do that. Take a look at the following example:

![Header example](postman_header_example.PNG)

In this example, we are making a call to the `/profile` end point in the API. 
We are making the request type `PATCH`. 
When sending `PATCH` request, it is usual to an a body, like in the `/login` request from a previous task.
There are two headers in this request, a header specifying that the content type we send is in the JSON format,
and another header `Authorization` where the JWT token is added as a **Bearer token**.
The JWT token may be sent as an `X-Access-Token` as well, withouth the word `Bearer` in front of the token. 

Now, take this token and use it to get into the admin profile on the [web page](http://noob3-4.tghack.no). If you use all the knowledge you've gained from the modules so far, you should manage this task. It might be a bit tricky, though.. Go for it!
```
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE1MjA3MDMwMTc4NjMsImV4cCI6MTUyNDMwMzAxNywic3ViIjoiYWRtaW4iLCJpc3MiOiJtYXJpdGlvX28ifQ.RrP9Tf3Z6qTCDeEYJS20hQlLVl1eVESh_6RCpPbHhug
```

> Did you find the flags? Here is the solution:
>
> By getting a JWT token, we get a hint that we need to insert the token as an HTTP header when making a call to the API.
>
> A strategy is to find the token in the frontend in order to see how it is used in the system.
> When checking the network tab in the `Inspect` page, we see that the JWT is sent in the HTTP header
> as an `X-Access-Token`. Another strategy is to try out the most common implementations.
>
> Most likely, the backend reads the token if we use `X-Access-Token` as the header key.
> Using Postman, we may set this header key and add the JWT token provided in the task as a value, and then send the request. 
> The request URL and request type may be found in the network tab when logging in with any user.
> If it still is hard to solve this task, come by the Creative Lounge and get some help!
> 
> By sending the PATCH request to the API with the token as an `X-Access-Token`, you get the flag in response.
