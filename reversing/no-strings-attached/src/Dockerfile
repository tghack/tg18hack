FROM ubuntu:16.04

RUN apt update && apt install -y build-essential
COPY Makefile checker.c /tmp/
WORKDIR /tmp
RUN make
