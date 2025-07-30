option(INTERACTIVE_TRANSLATION "Open linguist for translating strings" ON)

# Update single ts file
macro(updateTs TARGET FILE SOURCE_DIR)
    message(STATUS "Updating ${FILE} from: ${SOURCE_DIR}")

    set(_args ${FILE} ${SOURCE_DIR})
    if (INTERACTIVE_TRANSLATION)
        list(APPEND _args "true")
    endif()

    set(ENV{LINGUIST} "${LINGUIST_EXECUTABLE}")
    set(ENV{LUPDATE} "${LUPDATE_EXECUTABLE}")
    if (UNIX)
        execute_process(COMMAND ${CMAKE_SCRIPTS_PATH}/tools/update_ts.sh ${_args}
            RESULT_VARIABLE result_code ERROR_VARIABLE error_text)
    else()
        execute_process(COMMAND bash tools/update_ts.sh ${_args}
            RESULT_VARIABLE result_code 
            ERROR_VARIABLE error_text 
            WORKING_DIRECTORY ${CMAKE_SCRIPTS_PATH})
    endif()

    if (NOT result_code STREQUAL "0")
        message(FATAL_ERROR "Error while updating ${FILE}. ${error_text}")
    endif()
endmacro()

# Update single po file
# DOMAIN marker for extracting strings from sorces
# SOURCE_DIR dir with sources
# TARGET target (use for output file name)
macro(updatePo DOMAIN SOURCE_DIR TARGET)
    message(STATUS "Updating PO file from: ${SOURCE_DIR}")

    set(_args ${DOMAIN} ${SOURCE_DIR} ${TARGET})
    if (INTERACTIVE_TRANSLATION)
        list(APPEND _args "true")
    endif()

    if (UNIX)
        execute_process(COMMAND ${CMAKE_SCRIPTS_PATH}/tools/update_po.sh ${_args}
            RESULT_VARIABLE result_code ERROR_VARIABLE error_text)
    else()
        execute_process(COMMAND bash tools/update_po.sh ${_args}
            RESULT_VARIABLE result_code 
            ERROR_VARIABLE error_text
            WORKING_DIRECTORY ${CMAKE_SCRIPTS_PATH})
    endif() 
  
    if (NOT result_code STREQUAL "0")
        message(FATAL_ERROR "Error while updating PO: ${SOURCE_DIR}. ${error_text}")
    endif()
endmacro()

macro(compileTsFiles TARGET PATH COMPONENT)
    set (FILES ${ARGN})
    # Handle Qt files
    if(FILES)
        printMessage(VERBOSE "Processing TS files: " "${FILES}")
        set(qm "${CMAKE_CURRENT_BINARY_DIR}/${TRANSLATION_DOMAIN}_ru.qm")

        add_custom_command(OUTPUT ${qm} COMMAND ${LRELEASE_EXECUTABLE}
            ARGS ${FILES} -qm ${qm} DEPENDS ${FILES})

        add_custom_target(${TARGET}_ts ALL DEPENDS ${qm} SOURCES ${FILES})
        install(FILES ${qm} DESTINATION ${PATH} COMPONENT ${COMPONENT})
    endif()
endmacro()

macro(compilePoFiles TARGET PATH COMPONENT)
    set (FILES ${ARGN})
    if (FILES)
        find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
        find_program(GETTEXT_MSGCAT_EXECUTABLE msgcat)
        if (NOT GETTEXT_MSGFMT_EXECUTABLE)
            message(FATAL_ERROR "No msgfmt found")
        endif()
        if (NOT GETTEXT_MSGCAT_EXECUTABLE)
            message(FATAL_ERROR "No msgcat found")
        endif()

        set(_tmpPo ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_.po)
        set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.gmo)

        add_custom_command(OUTPUT ${_gmoFile}
            COMMAND ${GETTEXT_MSGCAT_EXECUTABLE} -u --no-location -o ${_tmpPo} ${FILES}
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_tmpPo}
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            DEPENDS ${FILES})

        add_custom_target(${TARGET}_po ALL DEPENDS ${_gmoFile} SOURCES ${FILES})

        install(FILES ${_gmoFile}
            DESTINATION ${PATH}/ru_RU/LC_MESSAGES/ RENAME ${TARGET}.mo
            COMPONENT ${COMPONENT})
    endif()
endmacro()

macro(generateDomains PATH SRC DOMAINS)
    # Create domain file from template
    string (REPLACE ";" "\")(\"" TR_DOMAINS "${DOMAINS}")
    configure_file(${CMAKE_SCRIPTS_PATH}/templates/domains.h.in "${PATH}/Domains.h" @ONLY)
    configure_file(${CMAKE_SCRIPTS_PATH}/templates/domains.cpp.in "${PATH}/Domains.cpp" @ONLY)
    list(APPEND SRC ${PATH}/Domains.cpp)
endmacro()

function(compileTranslations TRANSLATION_COMPONENT)
    if(NOT TRANSLATION_PATH)
        message(FATAL_ERROR "Translations path is empty. Aborting.")
    endif(NOT TRANSLATION_PATH)

    set(ts_files ${${TRANSLATION_DOMAIN}_TS_FILES})
    set(po_files ${${TRANSLATION_DOMAIN}_PO_FILES})
    message(STATUS "Compiling translations for: ${TRANSLATION_DOMAIN}")
    if(ts_files)
        printMessage(VERBOSE "TS_FILES:")
        printList(VERBOSE "${ts_files}" INDENT)
    endif()
    if(po_files)
        printMessage(VERBOSE "PO_FILES:")
        printList(VERBOSE "${po_files}" INDENT)
    endif()

    # FIXME: It breaks relative path installation cases, e.g. Windows
    #convertRelativePath(${TRANSLATION_PATH} INSTALL_TRANSLATION_PATH)
    set(INSTALL_TRANSLATION_PATH ${TRANSLATION_PATH})
    message(STATUS "Install translations to " ${INSTALL_TRANSLATION_PATH})

    if(ts_files)
        compileTsFiles(${TRANSLATION_DOMAIN} ${INSTALL_TRANSLATION_PATH} ${TRANSLATION_COMPONENT} ${ts_files})
    endif()
    if(po_files)
        compilePoFiles(${TRANSLATION_DOMAIN} ${INSTALL_TRANSLATION_PATH} ${TRANSLATION_COMPONENT} ${po_files})
    endif()
endfunction(compileTranslations)

macro(i18n TARGET LOCALE SOURCE_DIR UPDATE_TRANSLATION)
    set(TS_FILE "${SOURCE_DIR}/${TARGET}_${LOCALE}.ts")
    if (EXISTS ${TS_FILE})
        if (${UPDATE_TRANSLATION})
            updateTs(${TARGET} ${TS_FILE} ${SOURCE_DIR})
        endif()

        list(APPEND ${TRANSLATION_DOMAIN}_TS_FILES ${TS_FILE})
        set(${TRANSLATION_DOMAIN}_TS_FILES ${${TRANSLATION_DOMAIN}_TS_FILES} PARENT_SCOPE)
    endif()

    if (${UPDATE_TRANSLATION})
        updatePo(${TRANSLATION_DOMAIN} ${SOURCE_DIR} ${TARGET})
    endif()

    set(PO_FILE "${SOURCE_DIR}/${TARGET}.po")
    if (EXISTS ${PO_FILE})
        list(APPEND ${TRANSLATION_DOMAIN}_PO_FILES ${PO_FILE})
        set(${TRANSLATION_DOMAIN}_PO_FILES ${${TRANSLATION_DOMAIN}_PO_FILES} PARENT_SCOPE)
    endif()
endmacro()
