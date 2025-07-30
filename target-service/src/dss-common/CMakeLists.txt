item(dss-common)

definePaths(${PROJECT_NAME})
generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

set(SOURCES "*.*")

staticLib(${PROJECT_NAME}
    GLOBBING "${SOURCES}"
    RECURSIVE true
    QT Network SerialBus SerialPort
    LIBS mpkjson mpkyaml mpkhttpws mpktypes
    COMPONENT ${PROJECT_NAME})

include_directories(${MODULES}/cpp-httplib)
