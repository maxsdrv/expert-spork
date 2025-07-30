#! /usr/bin/env bash
# -*- coding: utf-8 -*-

image=${STATIC_ANALYSIS_IMAGE:-"registry.dev.mpksoft.ru/mpk/ci/static-analysis-image:latest"}
ci_path=${CI_PATH:-"./ci"}

mount_point="/tmp/check"
build_point="/tmp/build"

cmake_args=${CMAKE_ARGS:-
   "-DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_QMAKE_EXECUTABLE=qmake \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON"}

$(dirname -- "$0")/check_image.sh ${image} && \
docker run --rm --volume=$(pwd):${mount_point}:ro \
        --mount type=tmpfs,destination=${build_point} \
        -e LOCAL=true\
        -e CHECK_FOLDERS=${CHECK_FOLDERS} \
        -e CHANGED_FILES_FILTER=${CHANGED_FILES_FILTER} \
        -e CHANGES_ONLY=${CHANGES_ONLY} \
        -e TARGET_BRANCH_NAME=${TARGET_BRANCH_NAME} \
        -e CHECK_SUBMODULES=${CHECK_SUBMODULES} \
        -e LINTER_DEBUG=${LINTER_DEBUG} \
        -e CMAKE_COMMAND="cmake -B${build_point} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${cmake_args}" \
        --entrypoint=/bin/bash ${image} \
        -xec "git config --global --add safe.directory \"*\" && \
            cd ${mount_point} && \
            \${CMAKE_COMMAND} && \
            ${ci_path}/scripts/run_clang-tidy.sh ${build_point} 2>&1"
