FROM ubuntu:22.04
MAINTAINER MPK Software <support@mpksoft.ru>
ENV DEBIAN_FRONTEND=noninteractive

RUN echo "resolvconf resolvconf/linkify-resolvconf boolean false" | \
        debconf-set-selections \
    && apt-get update \
    && apt-get install -y \
        git \
        findutils \
        grep \
        clang-format-14 \
        cmake-format \
    && apt-get clean \
    && ln -s $(which clang-format-14) /usr/bin/clang-format \
    && rm -rf /var/lib/apt/lists/*

 # Specific tools
#############  
RUN apt-get update                                                  \
    && ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime   \
    && apt-get install -y tzdata                                    \
    && dpkg-reconfigure --frontend noninteractive tzdata            \
    && apt-get install npm -y                                       \
    && npm install -g cspell@5.21.2                                 \
    && apt-get clean                                                \
    && rm -rf /var/lib/apt/lists/*

COPY ./scripts/* /usr/bin/
