set(TESTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tests)

if(NOT GMOCK_SOURCE)
    set(GMOCK_SOURCE ${CMAKE_SOURCE_DIR}/modules/googletest)
endif()
include(${CMAKE_SOURCE_DIR}/modules/gqtest/gqtest.cmake)
