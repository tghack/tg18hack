#!/bin/sh
rm -rf /var/cache/* /tmp/*
mkdir -p /var/cache/apk
apk add --update musl-dev
apk add --update go
go build -o /hack/serve /hack/proxy.go /hack/serve.go
apk del go musl-dev
rm -rf /var/cache/* /tmp/* /hack/*.*
