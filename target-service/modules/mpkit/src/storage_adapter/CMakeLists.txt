optional_item(mpkstorage_adapter OFF)

libraryTarget(
    ${PROJECT_NAME}
    INTERFACE
    GLOBBING "*"
    RECURSIVE true
    QT Core
    LIBS mpkboost
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/storage_adapter ${CMAKE_CURRENT_BINARY_DIR}/tests)
