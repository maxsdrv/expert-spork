optional_item(mpkuistatesaver OFF)

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    QT Gui Widgets
    DEPENDS mpkqt
    COMPONENT ${PROJECT_NAME})
