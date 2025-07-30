# @file
# @brief Set base install paths for project
#

# The paths are the folowing
# INSTALL_PREFIX   - base path for BINARIES and LIBRARIES (/usr/local)
# BINARY_PATH      - path to executable (/usr/local/bin)
# LIBRARY_PATH     - path to shared libraries (/usr/local/lib)
# CONFIG_PATH      - path to not changable config files, e.g.:
#                    (${INSTALL_PREFIX}/etc/${PACKAGE_NAME})
# CACHE_PATH       - path to changable configs, logs etc, e.g.:
#                    (${INSTALL_PREFIX}/var/lib/${PACKAGE_NAME})
# SHARED_PREFIX    - base path for plugins, help and translations, e.g.:
#                    (${INSTALL_PREFIX}/share)
# SHARED_PATH      - path to static application's data, like examples, docs or
#                    licenses, e.g.: (${SHARED_PREFIX}/${PACKAGE_NAME})
# PLUGIN_PATH      - path to application plugins, e.g.:
#                    (${INSTALL_PREFIX}/lib/${PACKAGE_NAME}/plugins)
# TRANSLATION_PATH - path to translation files, e.g.:
#                    (${INSTALL_PREFIX}/share/${PACKAGE_NAME}/translations)
# HELP_PATH        - path to help files, e.g.:
#                    (${INSTALL_PREFIX}/share/${PACKAGE_NAME}/doc)
# LOG_PATH         - path to log files, e.g.:
#                    (${INSTALL_PREFIX}/var/log/${PACKAGE_NAME})
# RUN_PATH         - path to run-time variable files, e.g.:
#                    (${INSTALL_PREFIX}/var/run/${PACKAGE_NAME})
# TMP_PATH         - path to temporary stuff, e.g.:
#                    (${INSTALL_PREFIX}/tmp)
# INCLUDE_PATH     - path to includes directory (if we install dev package), e.g.:
#                    (${INSTALL_PREFIX}/include)

# Change placeholders to project name
macro(doSubstitution VARNAME PROJECT_NAME OVERRIDABLE)
    if (NOT ORIGINAL_${VARNAME} STREQUAL ${VARNAME})
        string (REPLACE "%1" ${PROJECT_NAME} ${VARNAME} "${${VARNAME}}")
        set(RUNTIME_${VARNAME} "${${VARNAME}}")

        string (REGEX REPLACE "^\\.\\.(\\/)?" "" ${VARNAME} "${${VARNAME}}")
    endif()
    if (${OVERRIDABLE})
        if (NOT IS_ROOT)
            # Non-root components should be overriden
            set(${VARNAME} "${${VARNAME}}/${PROJECT_NAME}")
        else()
            # Top-level paths variables should be set to use them in postinst scripts
            set(METAPACKAGE_${VARNAME} ${${VARNAME}})
        endif()
    endif()
    set(ORIGINAL_${VARNAME} ${${VARNAME}})
    message("${VARNAME}=${${VARNAME}} (${RUNTIME_${VARNAME}})")
endmacro()

macro(preparePaths)
    doSubstitution(INSTALL_PREFIX ${PROJECT_NAME} FALSE)
    doSubstitution(BINARY_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(LIBRARY_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(SHARED_PREFIX ${PROJECT_NAME} FALSE)
    doSubstitution(CONFIG_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(INITD_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(PLUGIN_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(TRANSLATION_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(SHARED_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(HELP_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(CACHE_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(LOG_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(ICONS_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(SOUND_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(DUMPS_PATH ${PROJECT_NAME} TRUE)
    doSubstitution(RUN_PATH ${PROJECT_NAME} FALSE)
    doSubstitution(TMP_PATH ${PROJECT_NAME} FALSE)
endmacro()

# Create LOG_PATH and CACHE_PATH directories
macro(createWorkingDirs COMPONENT)
    install(DIRECTORY DESTINATION ${LOG_PATH} COMPONENT "${COMPONENT}")
    install(DIRECTORY DESTINATION ${CACHE_PATH} COMPONENT "${COMPONENT}")
endmacro()

macro(definePaths PROJECT_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(FORCE_USE_INSTALL_PREFIX true)
    else()
        # Workaround. Translations, docs and plugins should reside
        # in ${SOME_PATH}/${PROJECT_NAME} for release and ${SOME_PATH}
        # for debug
        if (NOT WIN32)
            set(PROJECT_SUFFIX "/%1")
        endif()
    endif()

    if(NOT DEFINED INSTALL_PREFIX)
        if(UNIX)
            if(CMAKE_BUILD_TYPE STREQUAL Debug)
                set(INSTALL_PREFIX "$ENV{HOME}/.${PROJECT_NAME}")
            else()
                set(INSTALL_PREFIX "/usr/local")
            endif()
        elseif(WIN32)
            if(CMAKE_BUILD_TYPE STREQUAL Debug)
                set(INSTALL_PREFIX "$ENV{USERPROFILE}/${PROJECT_NAME}")
            else()
                set(INSTALL_PREFIX "..")
            endif()
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
        STRING(REGEX REPLACE "\\\\" "/" INSTALL_PREFIX ${INSTALL_PREFIX})
    endif()

    if (NOT DEFINED BINARY_PATH)
        set(BINARY_PATH "${INSTALL_PREFIX}/bin")
    endif()

    if (NOT DEFINED LIBRARY_PATH)
        if(UNIX)
            set(LIBRARY_PATH "${INSTALL_PREFIX}/lib")
        elseif(WIN32)
            set(LIBRARY_PATH "${BINARY_PATH}")
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()
    if (CMAKE_BUILD_TYPE STREQUAL Debug AND NOT DEFINED CMAKE_INSTALL_RPATH)
       set(CMAKE_INSTALL_RPATH "${LIBRARY_PATH}")
    endif()


    if (NOT DEFINED CONFIG_PATH)
        if(UNIX)
            if (FORCE_USE_INSTALL_PREFIX)
                set(CONFIG_PATH "${INSTALL_PREFIX}/config")
            else()
                set(CONFIG_PATH "/etc/%1")
            endif()
        elseif(WIN32)
            set(CONFIG_PATH "${INSTALL_PREFIX}/config")
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()

    if(NOT DEFINED SHARED_PREFIX)
        if(UNIX)
            set(SHARED_PREFIX "${INSTALL_PREFIX}/share")
        elseif(WIN32)
            set(SHARED_PREFIX ${INSTALL_PREFIX})
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()

    if (NOT DEFINED PLUGIN_PATH)
        set(PLUGIN_PATH "${LIBRARY_PATH}${PROJECT_SUFFIX}/plugins")
    endif()

    if (NOT DEFINED INCLUDE_PATH)
        set(INCLUDE_PATH "${INSTALL_PREFIX}/include")
    endif()

    if (NOT DEFINED SHARED_PATH)
        set(SHARED_PATH "${SHARED_PREFIX}${PROJECT_SUFFIX}")
    endif()

    if (NOT DEFINED TRANSLATION_PATH)
        set(TRANSLATION_PATH "${SHARED_PREFIX}${PROJECT_SUFFIX}/translations")
    endif()

    if (NOT DEFINED HELP_PATH)
        set(HELP_PATH "${SHARED_PREFIX}${PROJECT_SUFFIX}/doc")
    endif()

    if (NOT DEFINED ICONS_PATH)
        set(ICONS_PATH "${SHARED_PREFIX}/icons${PROJECT_SUFFIX}")
    endif()

    if (NOT DEFINED SOUND_PATH)
        set(SOUND_PATH "${SHARED_PREFIX}/sounds${PROJECT_SUFFIX}")
    endif()

    if (NOT DEFINED CACHE_PATH)
        if(UNIX)
            if(FORCE_USE_INSTALL_PREFIX)
                set(CACHE_PATH "${INSTALL_PREFIX}/cache")
            else()
                set(CACHE_PATH "/var/lib/%1/cache")
            endif()
        elseif(WIN32)
            set(CACHE_PATH "${INSTALL_PREFIX}/cache")
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()

    if (NOT DEFINED LOG_PATH)
        if(UNIX)
            if(FORCE_USE_INSTALL_PREFIX)
                set(LOG_PATH "${INSTALL_PREFIX}/log")
            else()
                set(LOG_PATH "/var/log/%1")
            endif()
        elseif(WIN32)
            set(LOG_PATH "${INSTALL_PREFIX}/log")
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()

    if (NOT DEFINED DUMPS_PATH)
        set(DUMPS_PATH "${CACHE_PATH}/dumps")
    endif()

    if(NOT DEFINED INITD_PATH AND UNIX)
        if(FORCE_USE_INSTALL_PREFIX)
            set(INITD_PATH "${INSTALL_PREFIX}/init.d")
        else()
            set(INITD_PATH "/etc/init.d")
        endif()
    endif()

    if(NOT DEFINED SYSTEMD_PATH AND UNIX)
        if(FORCE_USE_INSTALL_PREFIX)
            set(SYSTEMD_PATH "${INSTALL_PREFIX}/systemd")
        else()
            set(SYSTEMD_PATH "/etc/systemd")
        endif()
    endif()

    if(NOT DEFINED RUN_PATH AND UNIX)
        if(FORCE_USE_INSTALL_PREFIX)
            set(RUN_PATH "${INSTALL_PREFIX}/run")
        else()
            set(RUN_PATH "/var/run/%1")
        endif()
    endif()

    if(NOT DEFINED TMP_PATH)
        if(UNIX)
            if(FORCE_USE_INSTALL_PREFIX)
                set(TMP_PATH "${INSTALL_PREFIX}/tmp")
            else()
                set(TMP_PATH "/tmp/%1")
            endif()
        elseif(WIN32)
            set(TMP_PATH "${INSTALL_PREFIX}/tmp")
        else()
            message(FATAL_ERROR "Unsupported OS")
        endif()
    endif()

    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    preparePaths()
endmacro()

#Generates Paths.h
macro(generatePaths PATH SRC)
    preparePaths()
    configure_file(${CMAKE_SCRIPTS_PATH}/templates/paths.h.in ${PATH}/Paths.h @ONLY)
endmacro()
