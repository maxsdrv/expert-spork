FROM silkeh/clang:17-bookworm AS builder

ARG LINTER_PACKAGES="git clazy"
COPY examples/dev-packages.apt .

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends --no-install-suggests \
    $LINTER_PACKAGES \
    $(cat dev-packages.apt) \
    && apt-get clean \
    && rm -rf /var/cache/apt /var/lib/apt/lists/* package.list

WORKDIR /app
COPY modules modules
COPY cmake cmake
COPY CMakeLists.txt .
COPY src src
COPY examples/CMakeLists.txt examples/
COPY examples/tamerlan-cli examples/tamerlan-cli
COPY examples/dss-tamerlan-imitator examples/dss-tamerlan-imitator
COPY examples/dss-sensor-imitator examples/dss-sensor-imitator

ARG TARGETARCH
ARG IMAGE_REPO
WORKDIR /app/$IMAGE_REPO/$TARGETARCH/build

ARG VERSION
# keep built files in a docker volume cache
RUN --mount=type=cache,target=/app/$IMAGE_REPO/$TARGETARCH/build  cmake /app \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_TESTS=OFF \
  -DINSTALL_PREFIX=/app \
  -DCONFIG_PATH=/app \
  -DAVOID_LICENSECC=1 \
  -DVERSION=$VERSION

RUN --mount=type=cache,target=/app/$IMAGE_REPO/$TARGETARCH/build  cmake --build . --target dss-tamerlan-imitator --target dss-sensor-imitator --parallel "$(nproc)"
RUN --mount=type=cache,target=/app/$IMAGE_REPO/$TARGETARCH/build  cmake --install . --component dss-tamerlan-imitator
RUN --mount=type=cache,target=/app/$IMAGE_REPO/$TARGETARCH/build  cmake --install . --component dss-sensor-imitator

RUN mkdir debian && touch debian/control
RUN dpkg-shlibdeps /app/bin/dss-tamerlan-imitator -O | sed "s/^shlibs:Depends=\(.*\)/\1/" >> /app/package.lock
RUN echo "," >> /app/package.lock
RUN dpkg-shlibdeps /app/bin/dss-sensor-imitator -O | sed "s/^shlibs:Depends=\(.*\)/\1/" >> /app/package.lock

FROM debian:bookworm-slim
LABEL org.opencontainers.image.authors="MPK Software <support@mpksoft.ru>"

COPY --from=builder /app/package.lock .
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt satisfy -y \
        gettext-base \
        libqt5sql5-psql \
        "$(cat package.lock)" \
    && apt-get clean \
    && rm -rf /var/cache/apt /var/lib/apt/lists/* package.lock

WORKDIR /app

COPY --from=builder /app/bin/dss-tamerlan-imitator .
RUN ./dss-tamerlan-imitator --help

COPY --from=builder /app/dss-sensor-imitator-api.json .
COPY --from=builder /app/bin/dss-sensor-imitator .
RUN ./dss-sensor-imitator --help
