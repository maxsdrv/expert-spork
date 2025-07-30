FROM archlinux/archlinux:base
MAINTAINER MPK Software <support@mpksoft.ru>

## Arch Linux is a rolling release distribution, so a full update is recommended when installing new packages. In other words, we suggest to execute RUN pacman -Syu immediately after your FROM statement
RUN pacman -Syu --noconfirm

# install packages
#############
RUN pacman -S --noconfirm core/file \
        boost \
        boost-libs \
        clang \
        clazy \
        cmake \
        cppcheck \
        docker \
        docker-compose \
        diffutils \
        fakeroot \
        gcc \
        gettext \
        git \
        go \
        libzip \
        llvm \
        llvm-libs \
        make \
        qt6-5compat \
        qt6-base \
        qt6-declarative \
        qt6-charts \
        qt6-imageformats \
        qt6-multimedia \
        qt6-serialport \
        qt6-scxml \
        qt6-svg \
        qt6-tools \
        qt6-webchannel \
        qt6-webengine \
        qt6-websockets \
        qt6-webview \
        which \
        yaml-cpp \
        zip \
    && pacman -Scc --noconfirm

# Make qt6 visible for build system
ENV PATH=/usr/lib/qt6/bin:$PATH
