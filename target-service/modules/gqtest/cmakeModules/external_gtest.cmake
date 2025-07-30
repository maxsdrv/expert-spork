if (DEFINED EXTERNAL_GTEST)
    return()
endif()

set(EXTERNAL_GTEST "1")

find_package(Threads REQUIRED)

if (NOT DEFINED GMOCK_SOURCE)
    message(WARNING "No GMOCK_SOURCE defined. Testing is disabled")
    set(BUILD_TESTS OFF)
    return()
endif()


include(FetchContent)

if("${CMAKE_VERSION}" GREATER_EQUAL 3.28)
    FetchContent_Declare(
      googletest
      SOURCE_DIR       "${GMOCK_SOURCE}"
      INSTALL_COMMAND  ""
      UPDATE_COMMAND   ""
      CMAKE_CACHE_ARGS "-DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}"
                       "-DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}"
                       "${GTEST_OPTIONS}"
      EXCLUDE_FROM_ALL
    )

    FetchContent_MakeAvailable(googletest)
else()
    FetchContent_Declare(
      googletest
      SOURCE_DIR       "${GMOCK_SOURCE}"
      INSTALL_COMMAND  ""
      UPDATE_COMMAND   ""
      CMAKE_CACHE_ARGS "-DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}"
                       "-DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}"
                       "${GTEST_OPTIONS}"
    )

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
      FetchContent_Populate(googletest)
      add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# For backward compatibility
set(GTEST_LIBRARY gtest)
set(GTEST_MAIN_LIBRARY gtest_main)
set(GMOCK_LIBRARY gmock)
set(GMOCK_MAIN_LIBRARY gmock)


