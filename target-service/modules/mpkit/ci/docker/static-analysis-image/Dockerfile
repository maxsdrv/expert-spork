FROM archlinux/archlinux:base
MAINTAINER MPK Software <support@mpksoft.ru>

# install packages
#############
RUN pacman -Sy --noconfirm core/file \
        core/diffutils \
        core/fakeroot \
        core/make \
        core/which \
        core/gettext \
        extra/git \
        core/gcc \
        extra/clang \
        extra/llvm \
        extra/llvm-libs \
        extra/cmake \
        extra/cppcheck \
        extra/clazy \
    && pacman -Scc --noconfirm

RUN pacman -Sy --noconfirm extra/qt5-base \
        extra/qt5-charts \
        extra/qt5-declarative \
        extra/qt5-quickcontrols2 \
        extra/qt5-serialport \
        extra/qt5-svg \
        extra/qt5-tools \
        extra/qt5-translations \
        extra/qt5-webview \
        extra/qt5-websockets \
        extra/boost \
        extra/boost-libs \
        yaml-cpp \
    && pacman -Scc --noconfirm
