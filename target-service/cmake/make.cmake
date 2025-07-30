cmake_minimum_required(VERSION 3.7)
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

option(USE_DEPRECATED "Use deprecated functionalty" true)

# Read predefined configs for build from file
if (DEFINED PREDEFINED_CONFIGS)
    set(CONFIGS_FILE ${CMAKE_SOURCE_DIR}/${PREDEFINED_CONFIGS})

    if(EXISTS "${CONFIGS_FILE}")
        file(STRINGS "${CONFIGS_FILE}" ConfigContents)
        foreach(NameAndValue ${ConfigContents})
          # Strip leading spaces
          string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
          # Find variable name
          string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
          # Find the value
          string(REPLACE "${Name}=" "" Value ${NameAndValue})
          # Set the variable
          set(${Name} "${Value}")
          message(STATUS "Set property from ${PREDEFINED_CONFIGS}: ${Name}=${Value}")
        endforeach()
    else()
        message(FATAL_ERROR "${CONFIGS_FILE} not found")
    endif()
endif()

set(CMAKE_SCRIPTS_PATH ${CMAKE_CURRENT_LIST_DIR})
if(USE_DEPRECATED)
    include (${CMAKE_SCRIPTS_PATH}/test.cmake)
    include (${CMAKE_SCRIPTS_PATH}/make/deprecated.cmake)
endif()

# All latter definitions should be called only once in root module
if(__cmake_guard)
    return()
endif()
set(__cmake_guard true)

# This will be removed in the near future
if(USE_DEPRECATED)
    include (${CMAKE_SCRIPTS_PATH}/examples.cmake)
endif()

# Order is somewhat important
include (${CMAKE_SCRIPTS_PATH}/make/utils.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/compiler.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/platform.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/version.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/boost.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/qt.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/paths.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/configs.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/generation.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/files.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/i18n.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/items.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/targets.cmake)

# Add gsl includes
include_directories(${CMAKE_SOURCE_DIR}/modules/gsl/include)
