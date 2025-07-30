# Some utility macros
option(VERBOSE_CMAKE "More verbose output in Cmake processing" OFF)

# Display message if VERBOSE_CMAKE in ON
macro(debug)
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17")
        message(DEPRECATION "debug() is deprecated, use native message()")
    endif()
    if (VERBOSE_CMAKE)
        message(${ARGN})
    endif()
endmacro()

macro(printMessage LEVEL)
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17")
        message(${LEVEL} "${ARGN}")
    else()
        message(STATUS "${ARGN}")
    endif()
endmacro()

macro(printList LEVEL LIST)
    set(OPT INDENT)
    cmake_parse_arguments(OPTIONS "${OPT}" "" "" ${ARGN})
    foreach(ITEM ${LIST})
        if(OPTIONS_INDENT)
            set(ITEM "    ${ITEM}")
        endif()
        if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17")
            message(${LEVEL} "${ITEM}")
        else()
            message(STATUS "${ITEM}")
        endif()
    endforeach()
endmacro()


# Convert relative path to absolute
function(convertRelativePath path convertedPath)
    if (IS_ABSOLUTE ${path})
        set(${convertedPath} ${path} PARENT_SCOPE)
    else()
        set(${convertedPath} ${BINARY_PATH}/${path} PARENT_SCOPE)
    endif()
endfunction(convertRelativePath)

#Print content of passed variables
macro(printVars MESSAGE)
    message(STATUS ${MESSAGE})
    set(VARIABLES ${ARGN})
    foreach (variable ${VARIABLES})
        if(NOT DEFINED ${variable})
            message(STATUS "\t${variable}=<NOT SET>")
        else()
            message(STATUS "\t${variable}=${${variable}}")
        endif()
    endforeach()
endmacro()

function(strip_debug_symbols TARGET)
    add_custom_command(
        TARGET "${TARGET}" POST_BUILD
        DEPENDS "${TARGET}"
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:${TARGET}>
                $<TARGET_FILE:${TARGET}>.debug)
    add_custom_command(
        TARGET "${TARGET}" POST_BUILD
        DEPENDS "${TARGET}"
        COMMAND $<$<CONFIG:relwithdebinfo>:${CMAKE_STRIP}>
        ARGS --only-keep-debug $<TARGET_FILE:${TARGET}>.debug
    )
    add_custom_command(
        TARGET "${TARGET}" POST_BUILD
        DEPENDS "${TARGET}"
        COMMAND $<$<CONFIG:relwithdebinfo>:${CMAKE_STRIP}>
        ARGS --strip-debug $<TARGET_FILE:${TARGET}>
    )

    install(FILES $<TARGET_FILE:${TARGET}>.debug
        DESTINATION ${BINARY_PATH}/debug
        COMPONENT ${PROJECT_NAME}_debug)
endfunction()

# Set option for root project
macro(setOption option value)
    if (IS_ROOT)
        set(${option} ${value} CACHE BOOL "Set ${option} to OFF" FORCE)
    endif()
endmacro()

#Add property to parent scope and define it for current scope either
macro (parentProperty name value)
    if (NOT ${name})
        # Protect from annoying warning about parent scope
        get_directory_property(hasParent PARENT_DIRECTORY)
        if(hasParent)
            set(${name} ${value} PARENT_SCOPE)
        endif()

        set(${name} ${value})
    endif()
endmacro()
