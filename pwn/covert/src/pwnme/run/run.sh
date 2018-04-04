#!/bin/bash

docker kill dedup-pwnme
docker rm dedup-pwnme
#docker run -it --name pwnme -p 6060:6060 tg18/pwnme /bin/sh
docker run --name pwnme -p 6060:6060 tg18/dedup-pwnme
