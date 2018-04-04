#!/bin/bash

docker kill dedup-server
docker rm dedup-server
docker run --name dedup-server tg18/dedup-server
