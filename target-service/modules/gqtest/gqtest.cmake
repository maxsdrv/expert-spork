# @file
# @brief Adding test components
#
if (DEFINED GQTEST_ROOT)
    return()
endif()
set(GQTEST_ROOT ${CMAKE_CURRENT_LIST_DIR})

include(${GQTEST_ROOT}/cmakeModules/external_gtest.cmake)
include(${GQTEST_ROOT}/cmakeModules/gqtest.cmake)

add_subdirectory(${GQTEST_ROOT})

