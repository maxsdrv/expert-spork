if (__MPKIT_GUARD)
    return()
endif()
set(__MPKIT_GUARD TRUE)

set(MPKIT_PATH ${CMAKE_CURRENT_LIST_DIR})

include(${MPKIT_PATH}/src/boost/translate.cmake)
include(${MPKIT_PATH}/src/log/logger.cmake)

add_subdirectory(${MPKIT_PATH}/src)
