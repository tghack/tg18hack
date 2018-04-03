# Writeup Super Awesome Web Project

This task give you a link to [this](http://api.superawesome.tghack.no/api-docs) API documentation, 
with the following message:

```
hey yo! h3re is the l1nk to the API of my superawesome web project. Be nice with it.
```

There is not much to go on, except start looking at the documentation and learn how the API works.

We get a few hints from reading the documentation:

1. This API has scopes, which handles the authorization of a user.
2. There is a `/flag` endpoint with a `read:flag` scope. (That most likely is the one we want to use.)
3. The API requires JWT's, and it is therefore likely that the scopes are defined within the JWT.


Now, let us test the API with Postman. With the `/login` endpoint, we get a JWT token in response.
Open the [JWT reader](https://jwt.io), and checkout the structure of the JWT. 
We see the scope variables. The next step is to add the scope `read:flag` to the JWT.
However, we need the secret in order to do that!

Hmm... What to do? Let's look a bit further. 
What happens if we try calling any of the endpoints without the JWT?
You get a nice note from one of the developers in the project, 
with a username to a git repository. 
By doing some searching, you find the repository for the [github project](https://github.com/maritiren/superawesomeproject).

With the repository, you may read all the code AND the commits.
There is a commit that indicates that someone might have posted the secret on git. 
(NOOOOO SECRETS ON GIT PEOPLE!)
The commit says `Move secret to local file`. 
Basically, you can open any commit between that commit and when the secret was stored the first time,
and grep for it. TADA! The secret is: `55CCE41AF215358AE42C75745989BAFE33E48100791DG2F1F8BE3FA5654102AE`. 

Now paste your secret into the JWT generator, add the scope "read:flag", 
and paste your access token into Postman and call the `GET /flag` endpoint.

And then, we get the flag in return!

```
TG18{how_many_projects_on_github_have_commited_secrets?}
```
