optional_item(mpkhttpws OFF)

option(ENABLE_WS "Enable WebSocket support in mpkhttpws module" ON)

set(GLOBBING_PATTERN "_includes/httpws/*.*" "*.*" "_includes/httpws/json/*.*" "json/*.*")

if (ENABLE_WS)
    set (QT_WS WebSockets)
    list(APPEND GLOBBING_PATTERN "_includes/httpws/ws/*.*" "ws/*")
endif()

if (_use_mpkyaml)
    set(YAML_LIB mpkyaml)
    list(APPEND GLOBBING_PATTERN "_includes/httpws/yaml/*.*" "yaml/*")
endif()
libraryTarget(
    ${PROJECT_NAME} ${${PROJECT_NAME}_build_type}
    GLOBBING ${GLOBBING_PATTERN}
    RECURSIVE false
    QT Concurrent Core ${QT_WS}
    DEPENDS mpkjson mpklog mpkqt mpkutils ${YAML_LIB}
    COMPONENT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/modules/cpp-httplib)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} crypt)
endif()

tests(${MPKIT_PATH}/tests/httpws ${CMAKE_CURRENT_BINARY_DIR}/tests)
