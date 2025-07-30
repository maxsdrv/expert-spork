#! /usr/bin/env bash
# -*- coding: utf-8 -*-

script_dir=$(dirname -- "$0")

if [ ! -f ".cmake-format.py" ]
then
    exit 0
fi

export CHANGED_FILES_FILTER=${CHANGED_FILES_FILTER:-"\.cmake$|\CMakeLists.txt$"}

formatMode="--check"

if [ ! -z ${CMAKE_FORMAT_AUTO_FIX+x} ]
then
    formatMode="--in-place"
fi

# TODO: show diff between origin and formated instead '--check'
echo "cmake-format --version: $(cmake-format --version)" &&
$script_dir/run_script.sh cmake-format $formatMode
