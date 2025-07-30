optional_item(mpkstable_link OFF)

compileTranslations(${PROJECT_NAME})

set(QT_MODULES Core)

if("${QT_MAJOR}" STREQUAL "6")
    list(APPEND QT_MODULES StateMachine)
endif()

libraryTarget(
    ${PROJECT_NAME} ${${PROJECT_NAME}_build_type}
    GLOBBING "*.*"
    SOURCES ${SRC}
    RECURSIVE true
    QT ${QT_MODULES}
    DEPENDS mpkexceptions mpkjson mpklog mpkqt mpkyaml
    COMPONENT ${PROJECT_NAME})
