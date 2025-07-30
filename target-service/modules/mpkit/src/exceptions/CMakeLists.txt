optional_item(mpkexceptions OFF)

libraryTarget(${PROJECT_NAME}
    INTERFACE
    GLOBBING "*"
    RECURSIVE true
    BOOST exception
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/exceptions ${CMAKE_CURRENT_BINARY_DIR}/tests)
