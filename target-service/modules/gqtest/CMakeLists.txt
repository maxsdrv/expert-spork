# cmake 3.14 has FetchContent module available
cmake_minimum_required(VERSION 3.14)

project(gqtest)

if (NOT DEFINED QT_MAJOR)
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
    message(STATUS "Using Qt version: ${QT_VERSION}")
    if (QT_VERSION GREATER_EQUAL 6.0.0)
        set(QT_MAJOR 6)
    else()
        set(QT_MAJOR 5)
    endif()

    set(QT_NAME Qt${QT_MAJOR})
    add_compile_definitions(QT_MAJOR=${QT_MAJOR})
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmakeModules/external_gtest.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmakeModules/gqtest.cmake)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 14)
endif()

if (BUILD_TESTS)
    # Project main source
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
