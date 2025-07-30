optional_item(mpkutils OFF)

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    BOOST filesystem
    DEPENDS mpkexceptions
    COMPONENT ${PROJECT_NAME})

#tests(${TESTS_PATH}/utils ${CMAKE_CURRENT_BINARY_DIR}/tests)
