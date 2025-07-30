#! /usr/bin/env bash
# -*- coding: utf-8 -*-

build_dir=${1:-.build}
script_dir=$(dirname -- "$0")

clang-tidy --version && \
$script_dir/run_script.sh clang-tidy \
    --quiet \
    --extra-arg-before=-xc++-header \
    --warnings-as-errors='*' \
    -p=$build_dir
