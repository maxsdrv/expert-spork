#!/bin/bash

# @file
# @brief Verify all xml files in folder
#
# @copyright (C) 2019 MPK Software


echo "Checks xml validity"

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters. Use: xmlverify <path>"
    exit 1
fi

for a in $(find $1 -name *.xml); do
  xmllint --noout $a
done
