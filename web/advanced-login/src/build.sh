#!/bin/sh
chmod -R 777 /hack
go build -o /hack/init /hack/init.go

rm -rf /hack/mongodb/data
mkdir /hack/mongodb/data

rm -rf /hack/mongodb/log
touch /hack/mongodb/log

chown -R root:root /hack
chmod -R 555 /hack
chmod -R 777 /hack/mongodb/data
chmod -R 777 /hack/mongodb/log

chown -R root:shadow /etc/shadow
chown -R root:root /etc/passwd
chown -R root:root /etc/group

chmod -R 640 /etc/shadow
chmod -R 644 /etc/passwd
chmod -R 644 /etc/group
