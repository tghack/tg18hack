# vim:ft=Dockerfile
# TODO: static IP
FROM alpine:3.4
MAINTAINER PewZ
LABEL Description="TG18 Dedup Server"

ENV	SERVICE_USER=tghack \
	SERVICE_UID=10001 \
	SERVICE_GROUP=tghack \
	SERVICE_GID=10001 \
	SERVICE_BINARY=server

RUN	addgroup -g ${SERVICE_GID} ${SERVICE_GROUP} && \
	adduser -g "${SERVICE_NAME} user" -D -H -G ${SERVICE_GROUP} -s /sbin/nologin -u ${SERVICE_UID} ${SERVICE_USER} && \
	apk add --update libcap ca-certificates \
	&& update-ca-certificates

WORKDIR /

EXPOSE 4040

COPY ./${SERVICE_BINARY} /
COPY ./flag.txt /

RUN chmod +x /${SERVICE_BINARY} && \
    setcap 'cap_net_bind_service=+ep' /${SERVICE_BINARY}

RUN chown -R ${SERVICE_USER}:${SERVICE_GROUP} /${SERVICE_BINARY}

RUN chmod 744 /flag.txt

USER ${SERVICE_USER}

ENTRYPOINT ["/server", "0.0.0.0", "4040"]
