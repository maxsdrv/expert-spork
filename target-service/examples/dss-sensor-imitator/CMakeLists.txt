item(dss-sensor-imitator)

find_package(Qt5 COMPONENTS Core Positioning REQUIRED)

definePaths(${PROJECT_NAME})
generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

executable(${PROJECT_NAME}
    GLOBBING "*.*"
    BOOST program_options
    QT Core Positioning
    LIBS dss-core dss-backend mpkjson mpkqt mpklog mpkrest
    RECURSIVE true
    COMPONENT ${PROJECT_NAME})

configure_file(
    "_config/${PROJECT_NAME}-api.json.in"
    "${GENERATED_CONFIG}/${PROJECT_NAME}/${PROJECT_NAME}-api.json" @ONLY)

installProjectConfiguration(${PROJECT_NAME} ${CONFIG_PATH} ${PROJECT_NAME})
