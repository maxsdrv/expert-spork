project(tamerlan-cli)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(Qt5 COMPONENTS SerialBus REQUIRED)

generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

file(GLOB_RECURSE SERVICE_SOURCES "*.h" "*.cpp")
add_executable(${PROJECT_NAME} ${SERVICE_SOURCES})

find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
    pkg_check_modules(MODBUS REQUIRED libmodbus)
else()
    find_package(modbus REQUIRED)
    find_package(yaml-cpp REQUIRED)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    Boost::program_options
    ${MODBUS_LIBRARIES} ${YAML_CPP_LIBRARIES}
    mpklog mpkjson mpkqt mpkqttypes mpkexceptions
    Qt5::SerialBus)
#    drone-detector-core)

target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_CPP_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${YAML_CPP_LIBRARY_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${MODBUS_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${MODBUS_LIBRARY_DIRS})

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${BINARY_PATH}
    COMPONENT ${PROJECT_NAME})

set(TRANSLATION_DOMAIN ${PROJECT_NAME})
i18n(${PROJECT_NAME} "ru" ${CMAKE_CURRENT_SOURCE_DIR} TRUE)
compileTranslations(${PROJECT_NAME})
