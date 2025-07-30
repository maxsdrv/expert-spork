#!/bin/bash

# @file
# @brief Remove submodule from project
#
# @copyright (C) 2019 MPK Software

# This script removes submodule from project. To remove module resides in modules/someModule
# one should pass modules/someModule as a parameter


if [ "$#" -ne 1 ]; then
    echo "Remove git submodule. Use: remove_module.sh <submodule>"
    exit 1
fi

git config -f .git/config --remove-section submodule.$1
git config -f .gitmodules --remove-section submodule.$1

git add .
git commit

git rm --cached $1

rm -rf $1
rm -rf .git/modules/$1
