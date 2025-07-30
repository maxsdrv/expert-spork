# Define path for global generated includes and generate defs file
set(GENERATED_INCLUDES_DIR ${CMAKE_BINARY_DIR}/include)
include_directories(${GENERATED_INCLUDES_DIR})

configure_file(${CMAKE_SCRIPTS_PATH}/templates/defs.h.in
    ${GENERATED_INCLUDES_DIR}/Defs.h @ONLY)

configure_file(${CMAKE_SCRIPTS_PATH}/templates/platform.h.in
    ${GENERATED_INCLUDES_DIR}/Platform.h @ONLY)

# Define path for generated configs
set(GENERATED_CONFIG ${CMAKE_BINARY_DIR}/config)
set(GENERATED_CACHE ${CMAKE_BINARY_DIR}/cache)
set(GENERATED_SHARE ${CMAKE_BINARY_DIR}/share)
set(GENERATED_FILES ${CMAKE_BINARY_DIR}/generated)

# Generates Paths.h
macro(generatePathsHeader TARGET)
    set(singleArgs PATH PREFIX)
    cmake_parse_arguments(OPTIONS "" "${singleArgs}" "" ${ARGN})

    if(OPTIONS_PATH)
        set(OUTPUT_PATH ${OPTIONS_PATH})
    else()
        set(OUTPUT_PATH ${GENERATED_INCLUDES_DIR}/${TARGET})
    endif()

    target_include_directories(${TARGET} PRIVATE ${OUTPUT_PATH})

    if(OPTIONS_PREFIX)
        set(OUTPUT_PATH ${OUTPUT_PATH}/${OPTIONS_PREFIX})
    endif()

    preparePaths(${TARGET})
    configure_file(${CMAKE_SCRIPTS_PATH}/templates/paths.h.in
        ${OUTPUT_PATH}/Paths.h @ONLY)
endmacro()
