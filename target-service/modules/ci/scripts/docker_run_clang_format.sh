#! /usr/bin/env bash
# -*- coding: utf-8 -*-

image=${STATIC_CHECK_IMAGE:-"registry.dev.mpksoft.ru/mpk/ci/static-check-image:latest"}
ci_path=${CI_PATH:-"./ci"}

mount_point="/tmp/check"

# In AUTO_FORMAT mode clang-format modifies source code, so run docker container from current user
$(dirname -- "$0")/check_image.sh $image  &&  \
docker run --rm --volume=$(pwd):${mount_point}:rw \
    --user $(id -u):$(id -g) \
    -e CHECK_FOLDERS=${CHECK_FOLDERS} \
    -e CHANGED_FILES_FILTER=${CHANGED_FILES_FILTER} \
    -e CHANGES_ONLY=${CHANGES_ONLY} \
    -e TARGET_BRANCH_NAME=${TARGET_BRANCH_NAME} \
    -e CHECK_SUBMODULES=${CHECK_SUBMODULES} \
    -e LINTER_DEBUG=${LINTER_DEBUG} \
    -e AUTO_FORMAT=${AUTO_FORMAT:-false} \
    -e LOCAL=true \
    --entrypoint=/bin/bash $image \
    -xec "cd ${mount_point} && \
            ${ci_path}/scripts/run_clang-format.sh 2>&1"
