item(dss-core)

find_package(Qt5 COMPONENTS Core Positioning Sql REQUIRED)

generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

set(TRANSLATION_DOMAIN ${PROJECT_NAME})
i18n(${PROJECT_NAME} "ru" ${CMAKE_CURRENT_SOURCE_DIR} TRUE)
compileTranslations(${PROJECT_NAME})

set(LIBS_${PROJECT_NAME} mpkjson mpkutils mpkboost mpktypes mpkhttpws mpkrest http-service-models)
if(NOT AVOID_LICENSECC)
    list(APPEND LIBS_${PROJECT_NAME} licensecc_static)
endif()

staticLib(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE true
    QT Core Positioning Sql
    LIBS ${LIBS_${PROJECT_NAME}}
    COMPONENT ${PROJECT_NAME})

tests(${TESTS_PATH}/dss-core ${CMAKE_CURRENT_BINARY_DIR}/tests)
