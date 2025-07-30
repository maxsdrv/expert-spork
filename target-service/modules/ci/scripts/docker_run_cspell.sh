#! /usr/bin/env bash
# -*- coding: utf-8 -*-

image=${STATIC_CHECK_IMAGE:-"registry.dev.mpksoft.ru/mpk/ci/static-check-image:latest"}
ci_path=${CI_PATH:-"./ci"}

mount_point="/tmp/check"

$(dirname -- "$0")/check_image.sh $image  &&  \
docker run --rm --volume=$(pwd):${mount_point}:ro \
    -e CHECK_FOLDERS=${CHECK_FOLDERS} \
    -e CHANGED_FILES_FILTER=${CHANGED_FILES_FILTER} \
    -e CHANGES_ONLY=${CHANGES_ONLY} \
    -e TARGET_BRANCH_NAME=${TARGET_BRANCH_NAME} \
    -e CHECK_SUBMODULES=${CHECK_SUBMODULES} \
    -e LINTER_DEBUG=${LINTER_DEBUG} \
    -e LOCAL=true \
    --entrypoint=/bin/bash $image \
    -xec "git config --global --add safe.directory \"*\" && \
            cd ${mount_point} && \
            ${ci_path}/scripts/run_cspell.sh 2>&1"
