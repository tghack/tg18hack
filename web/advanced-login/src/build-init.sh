#!/bin/sh

apk add --no-cache go nodejs mongodb musl-dev

npm install mongodb 2> /dev/null
npm install express 2> /dev/null
npm install body-parser 2> /dev/null

mkdir -p /hack/mongodb
mkdir -p /hack/mongodb/data
touch /hack/mongodb/log

chmod -R 777 /hack
