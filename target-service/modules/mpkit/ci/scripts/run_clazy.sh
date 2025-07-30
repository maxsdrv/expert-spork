#! /usr/bin/env bash
# -*- coding: utf-8 -*-

build_dir=${1:-.build}
script_dir=$(dirname -- "$0")

clazy_rules_file=$script_dir/../configs/.clazy
if [[ -f .clazy ]]; then
    clazy_rules_file=.clazy
fi

clazy-standalone --version && \
$script_dir/run_script.sh clazy-standalone \
    --checks=$(< ${clazy_rules_file}) \
    --header-filter=$PWD/src/* \
    --ignore-dirs=$PWD/src/generated/* \
    -p=$build_dir
