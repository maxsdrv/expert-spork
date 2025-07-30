optional_item(mpkplugins OFF)

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    DEPENDS mpkexceptions mpkqt mpkboost
    QT Core
    COMPONENT ${PROJECT_NAME})

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} BASE_NAME ${PROJECT_NAME}
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}_export.h)

tests(${MPKIT_PATH}/tests/plugins ${CMAKE_CURRENT_BINARY_DIR}/tests)
