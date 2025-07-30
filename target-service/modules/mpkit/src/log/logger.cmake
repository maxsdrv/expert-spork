function(createCliLoggerConfig TARGET DESTINATION)
    createLoggerConfig(cli ${TARGET} ${DESTINATION})
endfunction()

function(createGuiLoggerConfig TARGET DESTINATION)
    createLoggerConfig(gui ${TARGET} ${DESTINATION})
endfunction()

function(createServiceLoggerConfig TARGET DESTINATION)
    createLoggerConfig(service ${TARGET} ${DESTINATION})
endfunction()

function(createLoggerConfig PREFIX TARGET DESTINATION)
    set(TEMPLATE_FILENAME "${MPKIT_PATH}/src/log/_config/${PREFIX}.log.ini.in")
    set(DESTINATION_FILENAME "${DESTINATION}/${TARGET}.log.ini")

    # Log verbosity sustitution variable
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(LOG_VERBOSITY "DEBUG")
    else()
        set(LOG_VERBOSITY "INFO")
    endif()
    configure_file(${TEMPLATE_FILENAME} ${DESTINATION_FILENAME} @ONLY)
endfunction()
