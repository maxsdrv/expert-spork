optional_item(mpkdate OFF)

libraryTarget(
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    COMPONENT ${PROJECT_NAME})
