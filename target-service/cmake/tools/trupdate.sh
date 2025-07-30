#!/bin/bash

# @file
# @brief     Recursively traverse over modules and call lupdate for them
#
# @copyright (C) 2019 MPK Softwatre


update()
{
    local DIRECTORY=$1
    local TS_FILE=$2

    if [ -f "$TS_FILE" ]; then
        out="$(${LUPDATE} $DIRECTORY -ts $TS_FILE -no-obsolete -no-ui-lines -locations none)"
        echo "$out"

        if [ "x${INTERACTIVE}" = "x-i" ] ; then
            new="$(echo "$out" | grep 'Found' | awk {'print $5'})"
            if [ "$new" != "(0" ] ; then
                ${LINGUIST} $TS_FILE
            fi
        fi
    fi
}

exit 1

TS_EXTENSION="_ru.ts"
DIRECTORY=${PWD}

INTERACTIVE=$1
LINGUIST=$2
if [[ ! ${LINGUIST} ]]; then
    LINGUIST=$(which linguist-qt4)
fi
if [[ ! ${LINGUIST} ]]; then
    LINGUIST=$(which linguist)
fi

LUPDATE=$(which lupdate-qt4)
if [[ ! ${LUPDATE} ]]; then
    LUPDATE=$(which lupdate)
fi
if [[ ! ${LUPDATE} ]]; then
    echo Error: lupdate not found
    exit 1
fi

if [ -d "${DIRECTORY}"/modules ]; then
    for MODULE_DIR in "$DIRECTORY"/modules/*
    do
      # The former is for newton the later is for tdc
      if [ -d "${MODULE_DIR}"/src ]; then
        update "$MODULE_DIR/src" "$MODULE_DIR/$(basename $MODULE_DIR)$TS_EXTENSION"
      else
	update "$MODULE_DIR" "$MODULE_DIR/$(basename $MODULE_DIR)$TS_EXTENSION"
      fi
    done
fi

# Do we need this?
if [ -d "${DIRECTORY}"/external ]; then
    for EXTERNAL_DIR in "$DIRECTORY"/external/*
    do
        update "$EXTERNAL_DIR" "$EXTERNAL_DIR/$(basename $EXTERNAL_DIR)$TS_EXTENSION"
    done
fi

if [ -d "${DIRECTORY}"/translations ]; then
    for TS_FILE in "${DIRECTORY}"/translations/*${TS_EXTENSION}
    do
        update "${DIRECTORY}"/src ${TS_FILE}
    done
fi

