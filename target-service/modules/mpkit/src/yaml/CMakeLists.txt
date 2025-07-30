optional_item(mpkyaml OFF)

find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
else()
    find_package(yaml-cpp REQUIRED)
endif()

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    QT Core
    LIBS ${YAML_CPP_LIBRARIES}
    COMPONENT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_CPP_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${YAML_CPP_LIBRARY_DIRS})

tests(${MPKIT_PATH}/tests/yaml ${CMAKE_CURRENT_BINARY_DIR}/tests)
