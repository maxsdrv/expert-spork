FROM alpine:3.20.2
LABEL org.opencontainers.image.authors="MPK Software <support@mpksoft.ru>"

RUN apk add pipx
RUN pipx install ansible-core==2.17.3
ENV PATH="/root/.local/bin:$PATH"

WORKDIR /provision

COPY provision/requirements.* .
#RUN pipx inject ansible-core -r requirements.txt
RUN ansible-galaxy collection install -r requirements.yaml

COPY provision .

ARG VERSION
LABEL org.opencontainers.image.version="$VERSION"

ENV VERSION=${VERSION}
CMD ["scripts/main.sh"]
