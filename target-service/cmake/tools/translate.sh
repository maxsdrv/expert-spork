#!/bin/bash

# @file
# @brief    Recursively traverse over modules and call gettext utilities for them.
#           Can be used in ineractive mode, with special tool launched, if 
#           untranslated strings found, or automatic mode returning positive exit code
#           in that case.
#
# @copyright (C) 2019 MPK Software


# Global configuration
EXTENSIONS="cpp h"
VERSION="1.0"

# Sanity checks
GREP=$(which grep)
[ -x "${GREP}" ] || exit ${LINENO}

SED=$(which sed)
[ -x "${SED}" ] || exit ${LINENO}

FIND=$(which find)
[ -x "${FIND}" ] || exit ${LINENO}

WC=$(which wc)
[ -x "${WC}" ] || exit ${LINENO}

CAT=$(which cat)
[ -x "${CAT}" ] || exit ${LINENO}

XGETTEXT=$(which xgettext)
[ -x "${XGETTEXT}" ] || exit ${LINENO}

MSGINIT=$(which msginit)
[ -x "${MSGINIT}" ] || exit ${LINENO}

MSGATTRIB=$(which msgattrib)
[ -x "${MSGINIT}" ] || exit ${LINENO}

MSGMERGE=$(which msgmerge)
[ -x "${MSGMERGE}" ] || exit ${LINENO}

# Functions
usage()
{
    echo "    Usage: $(basename $1) [<options>]"
    echo ""
    echo "    Options:"
    echo "      -i, --interactive <appname>       path to interactive application for translating"
    echo "      -l, --locale <locale>             target locale (default is ru_RU.UTF-8)"
    echo "      -q, --quiet                       silent mode"
    echo "      -m, --modules                     add submodules to translation"
    echo "      -h, --help                        print this help"
    echo ""

    exit $LINENO
}


parse_args()
{
    local script_name="$1"
    shift 1

    while (( "$#" ))
    do
        local opt="$1"
        shift 1

        case "$opt" in
        
        # TODO Will be used, when interactive tool will be defined
        -i|--interactive)
            INTERACTIVE_COMMAND="$1"
            ;;

        -l|--locale)
            TARGET_LOCALE="$1"
            ;;

        -q|--quiet)
            SILENT_MODE="true"
            ;;

	-m|--modules)
            ADD_MODULES="true"
            ;;
            
        *)
            usage $script_name
            ;;

        esac
        shift 1
    done
}

translate()
{
    local source_dir="$1/src"
    local cmakefile="$1/CMakeLists.txt"
    local output_dir="$1/translations"

    if [[ ! -e "${output_dir}" ]]; then
        return
    fi

    if [[ ! -f "${cmakefile}" ]]; then
        echo CMakeLists.txt not found for $1.
        return
    fi

    # One should add marker #@translation(name) to CMakeLists.txt file to process translations
    local target_name=$(${GREP} @translation ${cmakefile} | ${SED} 's/#@translation//g' | ${SED} 's/[()]//g')

    # File extensions where translatable symbols might exists
    for ext in ${EXTENSIONS}
    do
        if [[ -z "${conditions}" ]]; then
            conditions="-name *.${ext} "
        else
            conditions+="-or -name *.${ext} "
        fi
    done

    # Collect source files
    local src=$(${FIND} ${source_dir} ${conditions})

    # Create template .pot file from sources
    ${XGETTEXT} --omit-header --c++ --boost --keyword=${target_name^^}_TR:1 --keyword=${target_name^^}_TR_PL:1,2 --keyword=${target_name^^}_TR_CTX:1c,2 --default-domain=${target_name} --output-dir=${output_dir} --output=${target_name}.pot ${src}
    
    # Define convinience variables
    local template="${output_dir}/${target_name}.pot"
    local temp1="${output_dir}/${target_name}.tmp.pot"
    local temp2="${output_dir}/${target_name}.tmp.po"
    local output="${output_dir}/${target_name}.po"
    
    # Do not process modules that have directory, but no translations
    if [[ ! -f ${template} ]]; then
        return
    fi

    # Replace absolute paths
    local path_to_remove=$(echo $1 | ${SED} 's/\//\\\//g')
    ${SED} -e "s/${path_to_remove}\///g" ${template} > ${temp1}

    # Make .po file from template
    if [[ -f ${output} ]]; then
        # Merge newly created file with already existed
        ${MSGMERGE} --output-file=${output} --quiet ${output} ${temp1}
        if [[ ! ${SILENT_MODE} ]]; then
            echo "Successfully merged ${output}"
        fi
    else
        # Init fresh .po file
        ${MSGINIT} --no-translator --locale=${TARGET_LOCALE} --input ${temp1} --output-file=${temp2}
        # Replace template fields
        ${CAT} ${temp2} | ${SED} "s/THE PACKAGE'S COPYRIGHT HOLDER/MPK Software/g" | ${SED} "s/PACKAGE/${target_name}/g" | ${SED} "s/VERSION/${VERSION}/g" > ${output}
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
    return ${count}
}

# That is project root directory
PROJECT_DIR=$(dirname $(dirname $(readlink -f $0)))

# Parse arguments
parse_args $0 $@

# Setup default locale if not set
if [ -z "${TARGET_LOCALE}"]; then
    TARGET_LOCALE=ru_RU.UTF-8
fi

DIRS=("${PROJECT_DIR}")

if [[ ${ADD_MODULES} ]]; then
  DIRS+=("${PROJECT_DIR}/modules"/*)
fi

# Update project and its submodules
for TARGET in "${DIRS[@]}"
do
    echo "Processing dir ${TARGET}"
    if [[ -d $TARGET ]]; then
      translate "${TARGET}"
      let COUNT+=$?
    fi	
done

# Check for untranslated strings
if [[ ${COUNT} > 0 ]]; then
    # TODO Call interactive command here
    echo ${COUNT} untranslated strings.
    exit 1
fi

exit 0
