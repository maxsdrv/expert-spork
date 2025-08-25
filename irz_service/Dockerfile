ARG GO_VER
FROM --platform=$BUILDPLATFORM golang:${GO_VER}-alpine AS builder
ARG TARGETOS
ARG TARGETARCH

WORKDIR /go/src/app

RUN apk add git
ARG GOMOD_HOST
RUN --mount=type=secret,id=gomod_token echo "machine $GOMOD_HOST login oauth2 password $(cat /run/secrets/gomod_token)" > ~/.netrc
RUN git config --global http.sslVerify false
RUN go env -w GOINSECURE=$GOMOD_HOST

COPY go.* ./
RUN go mod download -x

ARG VERSION
ARG FLAGS

COPY internal internal
COPY *.go ./

ARG GOCACHE=/go/.build-cache
RUN --mount=type=cache,target=$GOCACHE CGO_ENABLED=0 GOOS=$TARGETOS GOARCH=$TARGETARCH go build -v -a -o main $FLAGS -ldflags "-s -w \
    -X $GOMOD_HOST/irz/tamerlan/dds-shared/go/config/info.VERSION=$VERSION \
    "


FROM alpine:3.20.6
LABEL org.opencontainers.image.authors="MPK Software <support@mpksoft.ru>"
ARG VERSION
LABEL org.opencontainers.image.version="$VERSION"

ARG APP_ID
COPY --from=builder /go/src/app/main ./$APP_ID
COPY configs/gen.bulat.sh .
COPY configs/dev.env .
#COPY docs docs

ENV APP_ID=$APP_ID
ENTRYPOINT ["/bin/sh", "-c", ". /gen.bulat.sh && exec /$APP_ID"]
CMD exec ./$APP_ID
