ARG GO_VER
FROM --platform=$BUILDPLATFORM golang:${GO_VER}-alpine AS builder
ARG TARGETOS
ARG TARGETARCH

WORKDIR /go/src/app

RUN apk add git
ARG GOMOD_HOST
ARG GOMOD_TOKEN
RUN echo "machine $GOMOD_HOST login oauth2 password $GOMOD_TOKEN" > ~/.netrc
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

ENV APP_ID=$APP_ID
CMD exec ./$APP_ID 
