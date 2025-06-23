item(dss-backend)

generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

set(TRANSLATION_DOMAIN ${PROJECT_NAME})
i18n(${PROJECT_NAME} "ru" ${CMAKE_CURRENT_SOURCE_DIR} TRUE)
compileTranslations(${PROJECT_NAME})

set(SOURCES "*.*")
list(APPEND SOURCES "exceptions/*")
set(QT_MODULES Core Positioning)
set(LIBS_MODULES dss-core mpkjson mpkutils mpkboost mpktypes mpklog mpkqt mpkyaml)

if(WITH_ALIA)
    add_definitions(-DUSE_ALIA)
    list(APPEND SOURCES "alia/*")
    list(APPEND QT_MODULES Network SerialBus SerialPort)
    list(APPEND LIBS_MODULES dss-common)
endif()

if(WITH_RADAR_IQ)
    add_definitions(-DUSE_RADAR_IQ)
    list(APPEND SOURCES "radariq/*")
    list(APPEND LIBS_MODULES dss-common mpkhttpws mpkrest radariq-client)
endif()

if(WITH_REST)
    add_definitions(-DUSE_REST)
    list(APPEND SOURCES "rest/*" "rest/*/*")
    list(APPEND LIBS_MODULES dss-common mpkhttpws mpkrest)
endif()

if(WITH_SKYCOPE)
    add_definitions(-DUSE_SKYCOPE)
    list(APPEND SOURCES "skycope/*")
    list(APPEND LIBS_MODULES dss-common skycope-client)
endif()

if(WITH_TAMERLAN)
    add_definitions(-DUSE_TAMERLAN)
    list(APPEND SOURCES "tamerlan/*" "tamerlan/modbus/*")

    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(MODBUS REQUIRED libmodbus)
    else()
        find_package(modbus REQUIRED)
    endif()

    list(APPEND LIBS_MODULES ${MODBUS_LIBRARIES})
endif()


find_package(Qt5 COMPONENTS "${QT_MODULES}" REQUIRED)

staticLib(${PROJECT_NAME}
    GLOBBING "${SOURCES}"
    RECURSIVE false
    QT "${QT_MODULES}"
    LIBS "${LIBS_MODULES}"
    COMPONENT ${PROJECT_NAME})

if(WITH_TAMERLAN)
    target_include_directories(${PROJECT_NAME} PUBLIC ${MODBUS_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PUBLIC ${MODBUS_LIBRARY_DIRS})
endif()

# tests(${TESTS_PATH}/dss-core ${CMAKE_CURRENT_BINARY_DIR}/tests)
