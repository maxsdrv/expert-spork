#! /usr/bin/env bash
# -*- coding: utf-8 -*-

build_type=${BUILD_TYPE:-Debug}

if [ -z ${BUILD_IMAGE+x} ]; then
    echo "Build image should be set, use 'env BUILD_IMAGE=<image> env BUILD_TYPE=<Debug|Release> ./ci/scripts/buils_in_docker.sh'"
    exit 1
fi

mount_point="/tmp/check"
build_point="/tmp/build"

cmake_args=${CMAKE_ARGS:-"-DCMAKE_INSTALL_PREFIX='' \
            -DINSTALL_PREFIX='..' \
            -DCONFIG_PATH='../etc' \
            -DINITD_PATH='../etc' \
            -DCACHE_PATH='../cache' \
            -DSHARED_PATH='../share' \
            -DPLUGIN_PATH='../lib/plugins' \
            -DHELP_PATH='../doc' \
            -DTRANSLATION_PATH='../translations' \
            -DTMP_PATH='../tmp' \
            -DBUILD_EXAMPLES=ON \
            -DBUILD_TESTS=ON ${CMAKE_EXTRA_ARGS}"}

$(dirname -- "$0")/check_image.sh ${BUILD_IMAGE} && \
docker run --rm --volume=$(pwd):${mount_point}:ro \
        --mount type=tmpfs,destination=${build_point} \
        -e BUILD_POINT=${build_point} \
        -e CMAKE_COMMAND="cmake -B${build_point} -DCMAKE_BUILD_TYPE=${build_type} ${cmake_args}" \
        --entrypoint=/bin/bash ${BUILD_IMAGE} \
        -xec "git config --global --add safe.directory \"*\" && \
            cd ${mount_point} && \
            \${CMAKE_COMMAND} && cmake --build \${BUILD_POINT} 2>&1"
