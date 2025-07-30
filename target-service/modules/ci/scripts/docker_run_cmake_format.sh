#! /usr/bin/env bash
# -*- coding: utf-8 -*-

image=${STATIC_CHECK_IMAGE:-"registry.dev.mpksoft.ru/mpk/ci/static-check-image:latest"}
ci_path=${CI_PATH:-"./ci"}

# In CMAKE_FORMAT_AUTO_FIX mode cmake-format modifies source code, so run docker container from current user
$(dirname -- "$0")/check_image.sh $image  &&  \
docker run --rm \
    --volume=$(pwd):/pwd \
    --user $(id -u):$(id -g) \
    -w /pwd \
    -e CHECK_FOLDERS=${CHECK_FOLDERS} \
    -e CHANGED_FILES_FILTER=${CHANGED_FILES_FILTER} \
    -e CHANGES_ONLY=${CHANGES_ONLY} \
    -e TARGET_BRANCH_NAME=${TARGET_BRANCH_NAME} \
    -e CHECK_SUBMODULES=${CHECK_SUBMODULES} \
    -e LINTER_DEBUG=${LINTER_DEBUG} \
    -e LOCAL=true \
    -e CMAKE_FORMAT_AUTO_FIX=${CMAKE_FORMAT_AUTO_FIX:-true} \
    --entrypoint=/bin/bash $image \
    -xec "${ci_path}/scripts/run_cmake-format.sh 2>&1"
