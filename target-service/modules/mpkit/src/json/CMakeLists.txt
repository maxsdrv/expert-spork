optional_item(mpkjson OFF)

set(_depends "")

if(_use_mpkdate)
    set(_depends mpkdate)
endif()

libraryTarget(
    ${PROJECT_NAME} INTERFACE
    GLOBBING "*"
    RECURSIVE true
    QT Core
    LIBS mpkexceptions mpkqttypes ${_depends}
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/json ${CMAKE_CURRENT_BINARY_DIR}/tests)
