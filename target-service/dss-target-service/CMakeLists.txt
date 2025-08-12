item(dss-target-service)

find_package(Qt5 COMPONENTS Core REQUIRED)

definePaths(${PROJECT_NAME})
generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

executable(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE true
    BOOST program_options
    QT Core
    LIBS mpklog dss-backend dss-core
    COMPONENT ${PROJECT_NAME})

createServiceLoggerConfig(${PROJECT_NAME} ${GENERATED_CONFIG}/${PROJECT_NAME})

# Create api config
configure_file("_config/dss-target-service-api.json.in"
    "${GENERATED_CONFIG}/${PROJECT_NAME}/dss-target-service-api.json" @ONLY)

installProjectConfiguration(${PROJECT_NAME} ${CONFIG_PATH} ${PROJECT_NAME})

#if (UNIX)
#    # Install init scripts
#    configure_file("_scripts/dss-target-service.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/scripts/dss-target-service" @ONLY)
#
#    # Install init script permissions
#    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/scripts/dss-target-service"
#            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
#                        GROUP_EXECUTE GROUP_READ
#                        WORLD_EXECUTE WORLD_READ
#            DESTINATION ${INITD_PATH}
#            COMPONENT ${PROJECT_NAME})
#
#    # Install init system default settings to /etc/default
#    install(DIRECTORY "_config/default"
#            DESTINATION "${CONFIG_PATH}/.."
#            COMPONENT ${PROJECT_NAME})
#
#    # Install systemd service
#    configure_file("_scripts/dss-target-service.service.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/scripts/dss-target-service.service" @ONLY)
#
#    # Install systemd service
#    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/scripts/dss-target-service.service"
#            DESTINATION "${SYSTEMD_PATH}/system"
#            COMPONENT ${PROJECT_NAME})
#
#    # Logrotate configuration
#    configure_file("_config/logrotate.d/dss-target-service.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/logrotate.d/dss-target-service" @ONLY)
#
#    # Install logrotate configuration
#    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/logrotate.d/dss-target-service"
#            DESTINATION "${CONFIG_PATH}/../logrotate.d"
#            COMPONENT ${PROJECT_NAME})
#
#    # Rsyslog configuration
#    configure_file("_config/rsyslog.d/00-dss-target-service.conf.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/rsyslog.d/00-dss-target-service.conf" @ONLY)
#
#    # Install rsyslog configuration
#    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rsyslog.d/00-dss-target-service.conf"
#            DESTINATION "${CONFIG_PATH}/../rsyslog.d"
#            COMPONENT ${PROJECT_NAME})
#endif()

if (NOT APPIMAGE_DEB)
    # Create cache folder
    install(DIRECTORY DESTINATION ${CACHE_PATH}
            DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                                  GROUP_EXECUTE GROUP_READ GROUP_WRITE
                                  WORLD_READ WORLD_WRITE WORLD_EXECUTE
            COMPONENT ${PROJECT_NAME})

    # Create logs folder
    install(DIRECTORY DESTINATION ${LOG_PATH}
            DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                                  GROUP_EXECUTE GROUP_READ GROUP_WRITE
                                  WORLD_READ WORLD_WRITE WORLD_EXECUTE
            COMPONENT ${PROJECT_NAME})
endif()
