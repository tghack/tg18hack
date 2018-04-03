# strange_signals writeup

In this taks you are given a location and some timestamps that a signal was seen.

2018-03-27 04:33:20
2018-03-27 10:06:42
2018-03-27 15:40:02
2018-03-27 21:13:24

If we convert these to UNIX epoch time, it is simpler to see the pattern:

```
$ python3

import time
times=["2018-03-27 04:33:20","2018-03-27 10:06:42","2018-03-27 15:40:02","2018-03-27 21:13:24"] 
for timest in times:
    print(time.mktime(time.strptime(timest,"%Y-%m-%d %H:%M:%S")))

1522118000.0
1522138002.0
1522158002.0
1522178004.0
```


The next expected epoch should be 1522198004, then 1522218006, 1522238006 etc.
Next expected: 1522178004 + 20000 = 1522198004
And the next after that one should be 1522198004 + 20000 + 2 = 1522218006
We can convert these epoch times to a more human readable time with a simple python oneliner:

```
$ python3 -c 'import time; print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(1522198004)))'
$ 2018-03-28 02:46:44
```

If we continue the pattern we get the full signal schedule:

```
epoch: 1522118000  human: 2018-03-27 04:33:20
epoch: 1522138002  human: 2018-03-27 10:06:42
epoch: 1522158002  human: 2018-03-27 15:40:02
epoch: 1522178004  human: 2018-03-27 21:13:24
epoch: 1522198004  human: 2018-03-28 02:46:44
epoch: 1522218006  human: 2018-03-28 08:20:06
epoch: 1522238006  human: 2018-03-28 13:53:26
epoch: 1522258008  human: 2018-03-28 19:26:48
epoch: 1522278008  human: 2018-03-29 01:00:08
epoch: 1522298010  human: 2018-03-29 06:33:30
epoch: 1522318010  human: 2018-03-29 12:06:50
epoch: 1522338012  human: 2018-03-29 17:40:12
epoch: 1522358012  human: 2018-03-29 23:13:32
epoch: 1522378014  human: 2018-03-30 04:46:54
epoch: 1522398014  human: 2018-03-30 10:20:14
epoch: 1522418016  human: 2018-03-30 15:53:36
epoch: 1522438016  human: 2018-03-30 21:26:56
epoch: 1522458018  human: 2018-03-31 03:00:18
epoch: 1522478018  human: 2018-03-31 08:33:38
epoch: 1522498020  human: 2018-03-31 14:07:00
epoch: 1522518020  human: 2018-03-31 19:40:20
```

If you are in the right time and place, you should see a new wireless access point with the flag:

```
TG18{wir3less-3aster-b34c0ns?}
```

You should have been able to see the access point name by using the native wifi manager in your OS. You could also use apps like inSSIDer or WiGLE to help with the wifi hunting.

