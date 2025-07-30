optional_item(mpkqttypes OFF)

libraryTarget(${PROJECT_NAME}
    INTERFACE
    GLOBBING "*"
    RECURSIVE true
    QT Core
    LIBS mpkexceptions
    COMPONENT ${PROJECT_NAME})

target_sources(mpkqttypes
  PRIVATE
    _includes/qttypes/Filesystem.h
)

tests(${MPKIT_PATH}/tests/qttypes ${CMAKE_CURRENT_BINARY_DIR}/tests)
