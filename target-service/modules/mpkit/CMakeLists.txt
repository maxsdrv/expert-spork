cmake_minimum_required(VERSION 3.10)
project(mpkit)

# Set cmake policy to keep normal variable if option is set (to properly handle
# mpkit items)
cmake_policy(SET CMP0077 NEW)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/make.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/examples.cmake)

set(IS_ROOT ON)
set(MPKIT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Remove this after upgrading gtest
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION
   GREATER_EQUAL 12.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
endif()

definePaths(${PROJECT_NAME})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_use_mpkboost ON)
    set(_use_mpkconfig ON)
    set(_use_mpkdate ON)
    set(_use_mpkexceptions ON)
    set(_use_mpkhttpws ON)
    set(_use_mpkjson ON)
    set(_use_mpklog ON)
    set(_use_mpkmetric_units ON)
    set(_use_mpkplugins ON)
    set(_use_mpkqt ON)
    set(_use_mpkqttypes ON)
    set(_use_mpkrest ON)
    set(_use_mpkrpc ON)
    set(_use_mpkstable_link ON)
    set(_use_mpkstorage_adapter ON)
    set(_use_mpktranslations ON)
    set(_use_mpktypes ON)
    set(_use_mpkuistatesaver ON)
    set(_use_mpkutils ON)
    set(_use_mpkyaml ON)
endif()

# Project main source
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

examples()
