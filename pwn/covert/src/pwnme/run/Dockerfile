# vim:ft=Dockerfile
FROM alpine:3.4

ENV	SERVICE_USER=tghack \
	SERVICE_UID=10001 \
	SERVICE_GROUP=tghack \
	SERVICE_GID=10001 \
	SERVICE_BINARY=pwnme

RUN	addgroup -g ${SERVICE_GID} ${SERVICE_GROUP} && \
	adduser -g "${SERVICE_NAME} user" -D -H -G ${SERVICE_GROUP} -s /sbin/nologin -u ${SERVICE_UID} ${SERVICE_USER} && \
	apk add --update socat libcap ca-certificates \
	&& update-ca-certificates

WORKDIR /

EXPOSE 6060

COPY ./${SERVICE_BINARY} /
COPY ./wrap.sh /

RUN chmod +x /${SERVICE_BINARY} && \
    setcap 'cap_setpcap=+ep' /${SERVICE_BINARY}
    #setcap 'cap_net_bind_service=+ep' /${SERVICE_BINARY} && \

RUN chmod +x /wrap.sh

RUN chown -R ${SERVICE_USER}:${SERVICE_GROUP} /${SERVICE_BINARY}
RUN chown -R ${SERVICE_USER}:${SERVICE_GROUP} /wrap.sh

USER ${SERVICE_USER}

ENTRYPOINT ["/bin/sh", "-c", "/wrap.sh"]
