FROM ubuntu:22.04
MAINTAINER MPK Software <support@mpksoft.ru>
ENV DEBIAN_FRONTEND=noninteractive

RUN echo "resolvconf resolvconf/linkify-resolvconf boolean false" |  \
        debconf-set-selections                                       \
    && apt-get update                                                \
    && apt-get install -y                                            \
          --no-install-recommends                                    \
          --no-install-suggests                                      \
        bridge-utils                                                 \
        clang                                                        \
        clang-tidy                                                   \
        clazy                                                        \
        cmake                                                        \
        dpkg-dev                                                     \
        file                                                         \
        g++-11                                                       \
        gcc-11                                                       \
        gettext                                                      \
        git                                                          \
        ifupdown                                                     \
        libboost-all-dev                                             \
        libssl-dev                                                   \
        lsb-release                                                  \
        pkg-config                                                   \
        resolvconf                                                   \
        valgrind                                                     \
    && apt-get clean                                                 \
    && rm -rf /var/lib/apt/lists/* 

