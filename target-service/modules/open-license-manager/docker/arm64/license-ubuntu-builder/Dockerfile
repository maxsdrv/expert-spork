FROM ubuntu:22.04
MAINTAINER MPK Software <support@mpksoft.ru>
ENV DEBIAN_FRONTEND=noninteractive \
    TARGET_HOST="aarch64-linux-gnu" \
    SYSROOT=/var/lib/sysroot-aarch64-linux-gnu

# Prepare build host
#############
RUN echo "resolvconf resolvconf/linkify-resolvconf boolean false" | \
        debconf-set-selections \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        crossbuild-essential-arm64 \
        debootstrap \
        gettext \
        git \
        lsb-release \
        pkg-config \
        protobuf-compiler \
        qemu-user \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Initialize sysroot
#############
RUN debootstrap --verbose --no-check-gpg --arch=arm64 --variant=minbase \
        --include=python3.10-minimal,libglib2.0-dev-bin jammy $SYSROOT

# Project specific libraries
#############
RUN echo "deb http://ports.ubuntu.com/ubuntu-ports jammy main" > \
        ${SYSROOT}/etc/apt/sources.list \
    && echo "deb http://ports.ubuntu.com/ubuntu-ports jammy universe" >> \
        ${SYSROOT}/etc/apt/sources.list \
    && echo "APT::Get::AllowUnauthenticated \"true\";" > \
            ${SYSROOT}/etc/apt/apt.conf.d/99unauth \
    && chroot $SYSROOT /bin/bash -c "export DEBIAN_FRONTEND=noninteractive \
        && rm -rf /usr/share/python3/runtime.d/python* \
        && rm -rf /usr/share/python3/runtime.d/libglib2.0* \
        && apt-get update \
        && apt-get install -y --no-install-recommends \
            libboost-filesystem-dev \
            libboost-program-options-dev \
            libboost-test-dev \
            libboost-thread-dev \
            libssl-dev \
            pkg-config \
        && apt-get clean" \
    && rm -rf ${SYSROOT}/var/lib/apt/lists/*
