# Historical Secrets Writeup

We get a gzipped archive as we can see on the file extension `.tgz`, which means that we need to unzip the archive to see the files. There are many ways to do this, but on linux and mac we can do this:

```sh
$ tar -xf historical_secrets.gzip
```

That gives us a `src` folder which we will take a look at:

```sh
$ cd src
$ ls -a
.  ..  .git  README.md  box  push.sh
```

We could take a look at the files, but since there is a `.git` folder here, it suggests that this folder is version controlled with [git](https://git-scm.com/).

>If you haven't used the `git` command before and don't know how to use a graphical user interface like [the github desktop client](https://desktop.github.com/), I recommend you install the `git` command if you want to follow along. Just search the web for `how install git mac` if you are using a mac, and so on.

>If you are new to git you may want to consult its man pages after you have installed it with `man git`.

To look at the git history we run:

```git
$ git log
commit b7666b89e921f0161e77dd56c11a43a72f845ac3 (HEAD -> master)
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:43:04 2018 +0100

    README says who created box

commit 0f5c40f95a8d5c98e2cf128170d71547e3c2ccd7
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:35:06 2018 +0100

    Remove password

commit 642be4b57791ebbf528746e0cec090e0f51ec3da
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:33:35 2018 +0100

    Add push script for integration testing

commit 2716c432843749463bbc8e23a299403aa092d5bc
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:27:00 2018 +0100

    Return whether lookup succeded.

commit 8b4c32421139f9f1fcbf057e981354dde78a54ef
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:12:30 2018 +0100

    Initial comit
```

Here we can see the different changes made to the files, or *commits* as
they're called in git speak. Each of these items contains a "diff"
(difference), also called "patch", that git uses to get from one version of the
files to another version.

We can ask `git` to see a diff from a commit by giving the commit id:

```git
$ git show 
commit b7666b89e921f0161e77dd56c11a43a72f845ac3 (HEAD -> master)
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:43:04 2018 +0100

    README says who created box

diff --git a/README.md b/README.md
index 96804f9..f8eaeff 100644
--- a/README.md
+++ b/README.md
@@ -12,3 +12,5 @@ if ok {
     fmt.Printf("%+v\n", thing)
 }
 \`\`\`
+
+Created by: Supreme Hacker
```

This diff says that an empty line and some text was added on line 15 and 16. The line with `fmt.Printf("%+v\n", thing)` is on line 12, which the line `@@ -12,3 +12,5 @@ if ok {` tells you, but I am digressing.

There wasn't anything interesting here, so we look at the next commit:

```git
$ git show 0f5c40f95a8d5c98e2cf128170d71547e3c2ccd7
commit 0f5c40f95a8d5c98e2cf128170d71547e3c2ccd7
Author: Supreme Hacker <supreme@hacker.hack>
Date:   Sun Jan 14 18:35:06 2018 +0100

    Remove password

diff --git a/push.sh b/push.sh
index 4ca439f..429d5ff 100644
--- a/push.sh
+++ b/push.sh
@@ -1,3 +1,3 @@
-echo "supremehacker" ; echo "TG18{SKELETONS_IN_MY_HISTORY}" | docker login
+echo "supremehacker" ; echo "${DOCKER_PASSWORD}" | docker login
 docker build -t "supremehacker/box:${BOX_IMAGE_TAG}" -f ../Dockerfile .
 docker push "supremehacker/box:${BOX_IMAGE_TAG}"

```

This commit is called `Remove password`. Interesting.
In the diff we can see a change that switches a password for an environment variable. That password is the token we're looking for!

```
TG18{SKELETONS_IN_MY_HISTORY}
```
