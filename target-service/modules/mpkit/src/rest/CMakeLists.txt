optional_item(mpkrest OFF)

if(NOT ENABLE_WS)
    message(FATAL_ERROR "WS in `httpws` is required to build `rest`. Either disable `rest` module or set `ENABLE_WS=ON`")
endif()

set(GLOBBING_PATTERN "_includes/mpk/rest/*.*" "*.*"
    "_includes/mpk/rest/json/*.*" "json/*.*"
    "_includes/mpk/rest/tag/*.*")

if(_use_mpkyaml)
    set(YAML_LIB mpkyaml)
    list(APPEND GLOBBING_PATTERN "_includes/mpk/rest/yaml/*.*" "yaml/*")
endif()

compileTranslations(${PROJECT_NAME})

libraryTarget(
    ${PROJECT_NAME} ${${PROJECT_NAME}_build_type}
    GLOBBING ${GLOBBING_PATTERN}
    RECURSIVE false
    SOURCES ${SRC}
    QT Core WebSockets
    DEPENDS mpkhttpws mpkstable_link ${YAML_LIB}
    COMPONENT ${PROJECT_NAME})

tests(${MPKIT_PATH}/tests/rest ${CMAKE_CURRENT_BINARY_DIR}/tests)
