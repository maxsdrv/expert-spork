include(CMakeParseArguments)

# Generate header file with translation macros
# Usage generateTranslationMacros(targetPath [DOMAIN_NAME domainName] [MACRO_NAME macroName])
# targetPath - Path where the header file will be generated [mandatory]
# domainName - Current project domain name (same as project name if not specified) [optional]
# macroName - Macro base name (uppercase project name if not specified [optional]
# NOTE: MPKIT_PATH must be specified to path where mpkit project is located
function(generateTranslationMacros TARGET TARGET_PATH)
    set(oneValueArgs DOMAIN_NAME MACRO_NAME)
    cmake_parse_arguments(TR "" "${oneValueArgs}" "" ${ARGN})

    if(NOT DEFINED TR_DOMAIN_NAME)
        set(TR_DOMAIN_NAME ${TARGET})
    endif()

    if(NOT DEFINED TR_MACRO_NAME)
        string(TOUPPER ${TARGET} TR_MACRO_NAME)
    endif()

    configure_file(${MPKIT_PATH}/src/boost/translate.h.in
        ${TARGET_PATH}/Translate.h @ONLY)
endfunction(generateTranslationMacros)
