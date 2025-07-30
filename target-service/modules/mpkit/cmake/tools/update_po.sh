#!/bin/bash

# @file
# @brief    Recursively traverse over modules and call gettext utilities for them.
#           Can be used in ineractive mode, with special tool launched, if 
#           untranslated strings found, or automatic mode returning positive exit 
#           code in that case.
#
# @copyright  (C) 2019 MPK Software


# Global configuration
EXTENSIONS="cpp h"
VERSION="1.2"

sanity=0
# Sanity checks
GREP=$(which grep 2>/dev/null)
[ -x "${GREP}" ] || { echo "'grep' not found" >&2; sanity=-1; }

SED=$(which sed 2>/dev/null)
[ -x "${SED}" ] || { echo "'sed' not found" >&2; sanity=-1; }

FIND=$(which find 2>/dev/null)
[ -x "${FIND}" ] || { echo "'find' not found" >&2; sanity=-1; }

WC=$(which wc 2>/dev/null)
[ -x "${WC}" ] || { echo "'wc' not found" >&2; sanity=-1; }

CAT=$(which cat 2>/dev/null)
[ -x "${CAT}" ] || { echo "'cat' not found" >&2; sanity=-1; }

XGETTEXT=$(which xgettext 2>/dev/null)
[ -x "${XGETTEXT}" ] || { echo "'xgettext' not found" >&2; sanity=-1; }

MSGINIT=$(which msginit 2>/dev/null)
[ -x "${MSGINIT}" ] || { echo "'msginit' not found" >&2; sanity=-1; }

MSGATTRIB=$(which msgattrib 2>/dev/null)
[ -x "${MSGATTRIB}" ] || { echo "'msgattrib' not found" >&2; sanity=-1; }

MSGMERGE=$(which msgmerge 2>/dev/null)
[ -x "${MSGMERGE}" ] || { echo "'msgmerge' not found" >&2; sanity=-1; }

[ ${sanity} -eq 0 ] || exit ${LINENO}

OUTPUT_PO=

translate()
{
    local marker="$1"
    local target="$3"
    local working_dir="$2"

    #echo "Working dir: $working_dir"
    #echo "Target name: $marker"
    #echo "Target: $target"
    
    if [[ ! -e "${working_dir}" ]]; then
        echo No output dir
        exit 1
    fi

    # File extensions where translatable symbols might exists
    for ext in ${EXTENSIONS}
    do
        if [[ -z "${conditions}" ]]; then
            conditions=(-name "*.${ext}")
        else
            conditions+=(-or -name "*.${ext}")
        fi
    done

    #echo "Conditions: $conditions"
    #echo "${FIND} ${working_dir} ${conditions}"
    
    # Collect source files
    local src=`${FIND} ${working_dir} "${conditions[@]}"`
    #echo $src

    # Create template .pot file from sources
    ${XGETTEXT} --omit-header --c++ --boost --keyword=${marker^^}_TR:1 --keyword=${marker^^}_TR_PL:1,2 --keyword=${marker^^}_TR_CTX:1c,2 --default-domain=${marker} --output-dir=${working_dir} --output=${target}.pot ${src}
    
    # Define convinience variables
    local template="${working_dir}/${target}.pot"
    local temp1="${working_dir}/${target}.tmp.pot"
    local temp2="${working_dir}/${target}.tmp.po"
    local output="${working_dir}/${target}.po"
    
    # Do not process modules that have directory, but no translations
    if [[ ! -f ${template} ]]; then
       echo "No template file found for ${target}"
       return
    fi

    # Replace absolute paths
    local path_to_remove=$(echo $2 | ${SED} 's/\//\\\//g')
    ${SED} -e "s/${path_to_remove}\///g" ${template} > ${temp1}

    # Make .po file from template
    if [[ -f ${output} ]]; then
        # Merge newly created file with already existed
        ${MSGMERGE} --output-file=${output} --quiet ${output} ${temp1}
    else
        # Init fresh .po file
        ${MSGINIT} --no-translator --locale=${TARGET_LOCALE} --input ${temp1} --output-file=${temp2}
        # Replace template fields
        ${CAT} ${temp2} | ${SED} "s/THE PACKAGE'S COPYRIGHT HOLDER/MPK SOFT/g" | ${SED} "s/PACKAGE/${target_name}/g" | ${SED} "s/VERSION/${VERSION}/g" > ${output}
    fi

    # Remove all obsolete items and make fuzzy items empty
    ${MSGATTRIB} --no-obsolete --clear-fuzzy --empty --output-file ${output} ${output}
    rm -f ${template} ${temp1} ${temp2} 1>/dev/null 2>&1

    # Return number of untranslated strings
    local count=$(${MSGATTRIB} --untranslated ${output} | ${GREP} msgid | ${WC} -l)
    # Subtract counted header msgid
    if [[ ${count} > 0 ]]; then
        let count-=1
    fi
    
    echo "Untranslated strings: " ${count}
    OUTPUT_PO=$output
}

# get_rtrn
#    get return result part
# params:
#    $1 - return result string
#    $2 - number of part to be returned
# returns:
#    return result part
function get_rtrn()
{
    echo `echo $1|cut --delimiter=, -f $2`
}

checkTranslation()
{
    local _FILE=$1
    local _INTERACTIVE=$2
    
    # Check for untranslated strings
    # We can use only implicit way to find untranslated string. 
    # We think that string is untranslated if we have 'msgstr ""' followed by empty string
    if $($GREP -A1 'msgstr ""' $_FILE | $GREP -q "^\$"); then
        if [[ ${_INTERACTIVE} ]]; then
            echo "Interactive mode" >&2
            ${LINGUIST} ${_FILE}
            # Do not use interactive for the second time
            checkTranslation ${_FILE}
        else
            echo "Untranslated strings in $_FILE" >&2
            exit 1
        fi        
    fi
}

updateTranslation()
{
    local _MARKER=$1
    local _DIR=$2
    local _OUTPUT=$3
    local _INTERACTIVE=$3
    
    # Update project and its submodules
    #echo "Processing dir ${_DIR}"
    if [[ -d ${_DIR} ]]; then
        translate "${_MARKER}" "${_DIR}" "${_OUTPUT}"
        
        if [[ $OUTPUT_PO ]]; then
            echo "Translation strings: $OUTPUT_PO"
            checkTranslation $OUTPUT_PO 1
        fi
    fi	
}

MARKER=$1
PROJECT_DIR=$2
OUTPUT_FILE=$3
INTERACTIVE=$4


TARGET_LOCALE=ru_RU.UTF-8

# MARKER is string for extracting strings (i.e coriolis)
# PROJECT_DIR is a working dir where script will search for sources
# OUTPUT_FILE is a name for output file (without extension and path)
# INTERACTIVE defines whether we should call linguist or not
updateTranslation $MARKER $PROJECT_DIR $OUTPUT_FILE $INTERACTIVE

exit 0
