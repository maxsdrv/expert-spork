#! /usr/bin/env bash
# -*- coding: utf-8 -*-

export CHANGED_FILES_FILTER="\.h$|\.hpp$|\.cpp$|\.qml$"
script_dir=$(dirname -- "$0")

cspell --version && \
$script_dir/run_script.sh cspell \
    --no-progress \
    --show-suggestions \
    --relative
