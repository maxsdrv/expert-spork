optional_item(mpkrpc OFF)

set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)
# FIXME: Rpc codebase should be updated to properly match private headers
set(RPC_PRIVATE_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)

list(APPEND lib_deps mpkboost mpkqt mpklog mpkjson)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

if(NOT UNIX)
    if(MSVC)
        list(APPEND lib_deps "bcrypt")
    elseif(MINGW)
        list(APPEND lib_deps "-lbcrypt")
    endif()
endif(NOT UNIX)

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    DEPENDS ${lib_deps}
    QT Core Network SerialPort
    COMPONENT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${RPC_PRIVATE_HEADERS_PATH})

# Mingw got problems with network based tests
if(UNIX)
    tests(${MPKIT_PATH}/tests/rpc ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif()
