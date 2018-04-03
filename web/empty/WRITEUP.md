# Empty Writeup

When opening the provided URL (https://empty.tghack.no), we're provided with a page that simply states the following:

```
This page is empty.
```

There's nothing interesting in the source code of the page, and the headers look mostly normal, except that a lot of security headers are turned on. With nothing to go on, we can look for common files like `robots.txt` and `sitemap.txt`.

`robots.txt` contains the following list of paths:
```
Disallow: /gammelt
Disallow: /system
Disallow: /HelloTheGathering
```

Both `gammelt` and `system` returns a 404 (not found), but `HelloTheGathering` presents us with the following page:

```
This page is temporary
```

However, if we look at the source by inspecting the page or using a tool like `curl`, we get the following:

```
$ curl https://empty.tghack.no/HelloTheGathering/
<h2>This page is temporary</h2>
<!-- Created by ~JohnyAdams -->
```

A convention when hosting per-user directories is to start the name with a tilde (`~`), so we can check if JohnyAdams has anything interesting in his folder:

```
$ curl https://empty.tghack/\~JohnyAdams/
Here's the flag: TG18{Inf0rmat1on_D1scl0sure_It's_Easy}
```

And there's the flag!
