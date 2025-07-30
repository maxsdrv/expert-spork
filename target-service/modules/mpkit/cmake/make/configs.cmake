# Functions for copy configs
function(installConfig TARGET DESTINATION COMPONENT)
    file(GLOB _files ${ARGN})

    # FIXME: It breaks relative path installation cases, e.g. Windows
    # convertRelativePath(${DESTINATION} INSTALL_CONFIG_PATH)
    set(INSTALL_CONFIG_PATH ${DESTINATION})
    install(FILES ${_files}
            DESTINATION ${INSTALL_CONFIG_PATH}
            COMPONENT "${COMPONENT}")

    add_custom_target(${TARGET}_config SOURCES ${_files})
endfunction(installConfig)

function(installRecursive TARGET DESTINATION COMPONENT DIRECTORY)
    file(GLOB FILES RELATIVE ${DIRECTORY} "${DIRECTORY}/*")
    foreach(FILE ${FILES})
        if(IS_DIRECTORY ${DIRECTORY}/${FILE})
            installRecursive(${TARGET} ${DESTINATION}/${FILE} ${COMPONENT} ${DIRECTORY}/${FILE})
        else()
            install(FILES ${DIRECTORY}/${FILE}
                    DESTINATION ${DESTINATION}
                    COMPONENT "${COMPONENT}")
            add_custom_target("${TARGET}_${FILE}" SOURCES ${DIRECTORY}/${FILE})
        endif()
    endforeach(FILE ${FILES})
endfunction(installRecursive)

# Install project configuration
macro(installProjectConfiguration TARGET DESTINATION COMPONENT)
    # This should be done before installing generated files
    # If we have METAPACKAGE_STATIC_CONFIG_PATH defined we are to check config files for TARGET
    # Use default path if no files exist
    if (METAPACKAGE_STATIC_CONFIG_PATH)
        set (TARGET_CONFIG_PATH "${METAPACKAGE_STATIC_CONFIG_PATH}/${TARGET}")
        if(EXISTS "${TARGET_CONFIG_PATH}")
            installConfig(${TARGET}_staticConfig ${DESTINATION} ${COMPONENT} "${TARGET_CONFIG_PATH}/*")
            set(${TARGET}ConfigsOverridden TRUE)
        endif()
    endif()

    if (NOT ${TARGET}ConfigsOverridden)
        installConfig(${TARGET}_staticConfig ${DESTINATION} ${COMPONENT} "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
    endif()

    installConfig(${TARGET}_generatedConfig ${DESTINATION} ${COMPONENT} "${GENERATED_CONFIG}/${TARGET}/*")

    if(IS_ROOT)
        installConfig(${TARGET}_nativeConfig ${DESTINATION} ${COMPONENT} "${CMAKE_CURRENT_SOURCE_DIR}/nativeconfig/*")
    endif()

endmacro()

# Install project configuration
macro(installProjectShares TARGET DESTINATION COMPONENT)
    installRecursive(${TARGET}_generatedShare ${DESTINATION} ${COMPONENT} ${GENERATED_SHARE}/${TARGET})
endmacro()

# Install project cache
macro(installProjectCache TARGET DESTINATION COMPONENT)
    installRecursive(${TARGET}_generatedCache ${DESTINATION} ${COMPONENT} ${GENERATED_CACHE}/${TARGET})
endmacro()

macro(installSounds TARGET COMPONENT)
    installConfig(${TARGET}_sounds  ${SOUND_PATH} ${COMPONENT} "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*")
endmacro()
