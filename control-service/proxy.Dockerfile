FROM alpine:3.20.2
LABEL org.opencontainers.image.authors="MPK Software <support@mpksoft.ru>"

RUN apk add nginx nginx-mod-http-upload gettext-envsubst

COPY configs/nginx.conf /etc/nginx/http.d/default.conf.template

ARG VERSION
LABEL org.opencontainers.image.version="$VERSION"

ENV VERSION=${VERSION}
CMD envsubst '\
     \$APP_ID \$VERSION \$PROJECT_ENV \$HOSTNAME \
     \$LISTEN_PORT \$LISTEN_UPLOAD \
     \$UPLOAD_DIR \$BACKUPS_DIR \
     ' < /etc/nginx/http.d/default.conf.template > /etc/nginx/http.d/default.conf \
     && exec nginx -g 'daemon off;'
