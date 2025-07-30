optional_item(mpkboost OFF)

set(DEPS mpktypes mpkexceptions)

if(LINK_RT)
    list(APPEND DEPS "rt")
endif()

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    BOOST locale
    DEPENDS ${DEPS}
    COMPONENT ${PROJECT_NAME})
