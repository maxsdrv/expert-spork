set(WARNING_FLAGS "-Wall -Wextra -Woverloaded-virtual -Winit-self -Wno-unused-local-typedefs")
if (NOT ALLOW_WARNINGS)
    set(WARNING_FLAGS "-Werror")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

if (NOT CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 17)
endif()

option(SANITIZE "Turn on address sanitizer" OFF)
option(COVERAGE "Turn on gcov" OFF)

# Sometimes CMAKE_BUILD_TYPE is not set if we don't pass -DCMAKE_BUILD_TYPE
# and the following expression breaks the compilation.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DEBUG_FLAGS "-O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # Turn on coverage build
    if(COVERAGE)
        set(COVERAGE_FLAGS "-pg --coverage")
        message(STATUS "Coverage is ON")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${COVERAGE_FLAGS}")
        set(GCOV ON)
    endif(COVERAGE)
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(_FLAGS "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS}")
else()
    set(RELEASE_FLAGS "-O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

# Turn on address sanitizer
if(SANITIZE)
    if (NOT DEFINED SANITIZERS)
        set(SANITIZERS "address,undefined,leak")
    endif()
    message(STATUS "Sanitizer is ON: ${SANITIZERS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZERS} -fno-omit-frame-pointer")
    set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=${SANITIZERS}")
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=${SANITIZERS}")
endif()

# Remove some warning depending of compiler
# CMAKE_CXX_COMPILER_ID is one of Clang, GNU, Intel or MSVC
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang Version: " ${CMAKE_CXX_COMPILER_VERSION})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "GCC Version: " ${CMAKE_CXX_COMPILER_VERSION})
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

message(STATUS "Compile flags: ${CMAKE_CXX_FLAGS}")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "ccache enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if (UNIX)
    add_definitions(-DOS_UNIX)
elseif(WIN32)
    add_definitions(-DOS_WIN32)
else()
    message(FATAL_ERROR "Usupported OS" )
endif(UNIX)
