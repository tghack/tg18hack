FROM alpine:latest

RUN ["/bin/mkdir", "-p", "/hack/webapp"]
COPY src/build-init.sh /hack/build-init.sh
RUN ["/bin/sh", "/hack/build-init.sh"]

COPY src/webapp/index.html /hack/webapp/index.html
COPY src/webapp/index.js /hack/webapp/index.js
COPY src/webapp/script.js /hack/webapp/script.js
COPY src/webapp/style.css /hack/webapp/style.css

COPY src/mongodb/mongodb.conf /hack/mongodb/mongodb.conf
COPY src/init.go /hack/init.go
COPY src/build.sh /hack/build.sh

COPY src/etc/passwd /etc/passwd
COPY src/etc/shadow /etc/shadow
COPY src/etc/group /etc/group

RUN ["/bin/sh", "/hack/build.sh"]

USER hack
CMD ["/hack/init"]
