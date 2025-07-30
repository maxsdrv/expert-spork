optional_item(mpktypes OFF)

set (_paths "_includes/types/*.*" "_includes/types/strong_typedef/*.*")

if (_use_mpkjson)
    list (APPEND _paths "_includes/types/strong_typedef/json/*.*")
    set (_depends mpkjson)
endif()

libraryTarget(${PROJECT_NAME}
    INTERFACE
    GLOBBING "${_paths}"
    RECURSIVE false
    LIBS ${_depends}
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/types ${CMAKE_CURRENT_BINARY_DIR}/tests)
