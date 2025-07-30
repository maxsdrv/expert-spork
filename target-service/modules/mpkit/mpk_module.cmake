if (__MPKIT_MODULE_DEFINED)
    return()
endif()
set(__MPKIT_MODULE_DEFINED TRUE)

set(MPKIT_PATH ${CMAKE_CURRENT_LIST_DIR})

include(${MPKIT_PATH}/src/boost/translate.cmake)
include(${MPKIT_PATH}/src/log/logger.cmake)
# TODO: Some mpkit modules (rpc for example) depends on this
include_directories(${MPKIT_PATH}/src)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 12.0)
    set(EXTRA_CXX_FLAGS "-Wno-maybe-uninitialized")
endif()

macro(add_mpkit_item ITEM)
    if(DEFINED EXTRA_CXX_FLAGS)
        set(SAVED_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")
    endif()

    add_item(mpk${ITEM} ${MPKIT_PATH}/src/${ITEM} ${ARGN})

    if(DEFINED SAVED_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS "${SAVED_CXX_FLAGS}")
    endif()
endmacro()
