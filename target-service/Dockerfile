FROM debian:bookworm-slim AS analyser

# Install common dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      gnupg2 wget ca-certificates apt-transport-https \
      git autoconf automake cmake dpkg-dev file make patch libc6-dev && \
    apt-get clean

# Install LLVM
RUN echo "deb https://apt.llvm.org/bookworm llvm-toolchain-bookworm-17 main" \
        > /etc/apt/sources.list.d/llvm.list && \
    wget -O /etc/apt/trusted.gpg.d/llvm.asc \
        https://apt.llvm.org/llvm-snapshot.gpg.key && \
    apt-get update && \
    apt-get install -y -t llvm-toolchain-bookworm-17 clang-17 clang-tidy-17 clang-format-17 lld-17 libc++-17-dev libc++abi-17-dev && \
    for f in /usr/lib/llvm-*/bin/*; do ln -sf "$f" /usr/bin; done && \
    ln -sf clang /usr/bin/cc && \
    ln -sf clang /usr/bin/c89 && \
    ln -sf clang /usr/bin/c99 && \
    ln -sf clang++ /usr/bin/c++ && \
    ln -sf clang++ /usr/bin/g++ && \
    apt-get clean

# Install dev dependencies
ARG LINTER_PACKAGES="clazy valgrind"
COPY dev-packages.apt .
RUN apt-get install -y --no-install-recommends \
      $LINTER_PACKAGES \
      $(cat dev-packages.apt) && \
    apt-get clean

FROM analyser AS builder

WORKDIR /app
COPY modules modules
COPY cmake cmake
COPY CMakeLists.txt .
COPY license license
COPY src src

ARG VERSION

# keep built files in a docker volume cache
ARG APP_ID
ARG TARGETARCH
ARG BUILD_DIR=/build/$APP_ID/$TARGETARCH
WORKDIR $BUILD_DIR

RUN --mount=type=cache,target=$BUILD_DIR,from=buildcache-in  cmake /app \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_TESTS=OFF \
  -DINSTALL_PREFIX=/app \
  -DCONFIG_PATH=/app \
  -DVERSION=$VERSION

RUN --mount=type=cache,target=$BUILD_DIR,from=buildcache-in  cmake --build . --parallel "$(nproc)"
RUN --mount=type=cache,target=$BUILD_DIR,from=buildcache-in  cmake --install .

RUN mkdir debian && touch debian/control
RUN dpkg-shlibdeps /app/bin/dss-target-service -O | sed "s/^shlibs:Depends=\(.*\)/\1/" \
      | sed -e s'/, /\n/g' | sort | sed -ze s'/\n/, /g' > /app/package.lock

WORKDIR /buildcache
RUN --mount=type=cache,target=$BUILD_DIR,from=buildcache-in \
    cp -r $BUILD_DIR/* . \
      && rm -rf licensecc-prefix/ # can't use this cache for production build as the stupid licensecc isn't rebuildable

FROM scratch AS buildcache-out
COPY --from=builder /buildcache /

FROM arigaio/atlas:0.36.0-alpine AS atlas
FROM debian:bookworm-slim
LABEL org.opencontainers.image.authors="MPK Software <support@mpksoft.ru>"
ARG VERSION
LABEL org.opencontainers.image.version="$VERSION"

COPY --from=builder /app/package.lock .
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get satisfy -y \
        gettext-base \
        libqt5sql5-psql \
        "$(cat package.lock)" \
    && apt-get clean \
    && rm -rf /var/cache/apt /var/lib/apt/lists/* package.lock

COPY --from=atlas /atlas /usr/bin/

WORKDIR /app

COPY entrypoint.sh .
COPY migrations migrations
COPY configs configs

COPY --from=builder /app/dss-target-service.log.ini .
COPY --from=builder /app/dss-target-service-api.json .

ARG APP_ID
COPY --from=builder /app/bin/dss-target-service ./$APP_ID
RUN ./$APP_ID --version

ENV APP_ID=$APP_ID
CMD migrations/apply.sh && exec ./entrypoint.sh ./$APP_ID
