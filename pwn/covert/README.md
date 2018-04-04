# Covert Flag Capturing Operation
**Points: 500**

**Author: PewZ**

Your mission is simple: pwn the service running at `covert.tghack.no:6060` and steal the flag from the local service running at `172.17.0.2:4040`.

* [service](server)
* [pwnme](pwnme)

### Hints
* No need to look for bugs in the server code
* pwnme and the server is separated by different docker containers, and can't access each other's files etc
