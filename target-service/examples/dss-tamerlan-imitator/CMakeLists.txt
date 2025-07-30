item(dss-tamerlan-imitator)

add_definitions(-DGSL_THROW_ON_CONTRACT_VIOLATION)

find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(MODBUS REQUIRED libmodbus)
else()
    find_package(modbus REQUIRED)
endif()

definePaths(${PROJECT_NAME})
generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

executable(${PROJECT_NAME}
    GLOBBING "*.*"
    BOOST program_options
    QT Core Network SerialBus SerialPort
    LIBS mpkqt mpklog mpkyaml mpkjson ${MODBUS_LIBRARIES}
    RECURSIVE true
    COMPONENT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${MODBUS_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${MODBUS_LIBRARY_DIRS})
