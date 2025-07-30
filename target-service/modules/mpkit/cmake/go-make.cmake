# @file
# @brief Combine several .cmake files
# Define if this project is root one

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
          message("Set property from ${PREDEFINED_CONFIGS}: ${Name}=${Value}")
        endforeach()
    else()
        message(FATAL_ERROR "${CONFIGS_FILE} not found")
    endif()
endif()

set(TESTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# All latter definitions should be called only once in root module
if(__cmake_definitions)
    return()
endif()
set(__cmake_definitions TRUE)

set(CMAKE_SCRIPTS_PATH ${CMAKE_CURRENT_LIST_DIR})

# Cmake utils
include(${CMAKE_SCRIPTS_PATH}/make/utils.cmake)

# Project definitions
if (NOT MODULES)
    set (MODULES ${CMAKE_SOURCE_DIR}/modules)
endif()

include(${CMAKE_SCRIPTS_PATH}/go-make/golang.cmake)

# Version
include (${CMAKE_SCRIPTS_PATH}/make/version.cmake)
defineVersion()

# Cmake build functions and macros
include (${CMAKE_SCRIPTS_PATH}/make/configs.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/i18n.cmake)
include (${CMAKE_SCRIPTS_PATH}/make/paths.cmake)

# Define path for global generated includes and generate defs file
set(GLOBAL_GENERATED_INCLUDES ${CMAKE_BINARY_DIR}/include)
include_directories(${GLOBAL_GENERATED_INCLUDES})

# The platform must be defined always
include (${CMAKE_SCRIPTS_PATH}/make/platform.cmake)

# Define path for generated configs
set(GENERATED_CONFIG ${CMAKE_BINARY_DIR}/config)
set(GENERATED_CACHE ${CMAKE_BINARY_DIR}/cache)
set(GENERATED_SHARE ${CMAKE_BINARY_DIR}/share)
