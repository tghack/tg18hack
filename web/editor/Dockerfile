FROM alpine:latest

RUN ["/bin/mkdir", "-p", "/hack/static/public"]
RUN ["/bin/mkdir", "-p", "/hack/static/private"]

COPY src/public/index.html /hack/static/public/index.html
COPY src/public/quill.js /hack/static/public/quill.js
COPY src/public/quill-snow.css /hack/static/public/quill-snow.css
COPY src/public/style.css /hack/static/public/style.css
COPY src/public/text-edit.js /hack/static/public/text-edit.js
COPY src/private/flag.txt /hack/static/private/flag.txt
COPY src/serve.go /hack/serve.go
COPY src/proxy.go /hack/proxy.go
COPY src/install.sh /hack/install.sh

RUN ["/bin/chmod", "-R", "755", "/hack"]
RUN ["/bin/busybox", "sh", "/hack/install.sh"]

USER nobody
CMD ["/hack/serve", "/hack/static"]
