#!/bin/bash

# @file
# @brief     Translate single ts file
#
# @copyright (C) 2019 MPK Software


TS_FILE=$1
DIRECTORY=$2
INTERACTIVE=$3

if [[ ! ${LINGUIST} ]]; then
    LINGUIST=$(which linguist-qt4)
fi
if [[ ! ${LINGUIST} ]]; then
    LINGUIST=$(which linguist)
fi

if [[ ! ${LUPDATE} ]]; then
    LUPDATE=$(which lupdate-qt4 2>/dev/null)
fi
if [[ ! ${LUPDATE} ]]; then
    LUPDATE=$(which lupdate 2>/dev/null)
fi
if [[ ! ${LUPDATE} ]]; then
    echo Error: lupdate not found >&2
    exit ${LINENO}
fi

echo "Using lupdate: ${LUPDATE}" 

checkTranslation()
{
    local _FILE=$1
    local _INTERACTIVE=$2
    
    if grep -q "<translation type=\"unfinished\">" $_FILE; then
        if [[ ${_INTERACTIVE} ]]; then
            echo "Interactive mode" >&2
            if [[ ${LINGUIST} ]]; then
                ${LINGUIST} $_FILE
                # Do not use interactive for the second time
                checkTranslation ${_FILE}
            else
                echo "No linguist found" >&2
                exit 1
            fi
        else
            echo "Untranslated strings in $_FILE" >&2
            exit 1
        fi
    fi    
}

update()
{
    local _TS_FILE=$1
    local _DIRECTORY=$2
    local _INTERACTIVE=$3
    
    if [ -f "${_TS_FILE}" ]; then
        out="$(${LUPDATE} ${_DIRECTORY} -ts ${_TS_FILE} -no-obsolete -no-ui-lines -locations none)"
        echo "$out"
        checkTranslation ${_TS_FILE} 1
    fi
}

update $TS_FILE $DIRECTORY $INTERACTIVE
