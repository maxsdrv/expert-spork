#!/bin/bash

# @file
# @brief Retrieve users commit statistics from git
#
# @copyright (C) 2019 MPK Software



if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters. Use: statistics.sh <username>"
    exit 1
fi

pushd $(dirname $0)/..

author=$1

fullemail="$author@dev.pkb-rio.com"
echo "Author: $fullemail "
git log --author=$fullemail --after=$1 --pretty=tformat: --numstat | \
grep -e 'cpp\|h' |  gawk '{ add += $1 ; subs += $2; total+=$1+$2 } END \
    { printf "Added lines: %s. Removed lines: %s. Total changes: %s \n",add,subs,total }'

popd
