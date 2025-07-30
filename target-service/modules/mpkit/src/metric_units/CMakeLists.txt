optional_item(mpkmetric_units OFF)

set(TRANSLATION_DOMAIN ${PROJECT_NAME})
i18n(${PROJECT_NAME} "ru" ${CMAKE_CURRENT_SOURCE_DIR} TRUE)
compileTranslations(${PROJECT_NAME})

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    QT Core
    DEPENDS mpkexceptions mpkboost mpktypes
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/metric_units ${CMAKE_CURRENT_BINARY_DIR}/tests)
