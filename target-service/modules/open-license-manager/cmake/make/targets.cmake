# @file
# @brief Add macros for several commonly used targets
#

include(CMakeParseArguments)

define_property(TARGET
    PROPERTY DEPENDS_OF
    BRIEF_DOCS "Target dependencies"
    FULL_DOCS "List of all dependent components for target"
    INHERITED)

# Link Qt module
macro(linkQt TARGET)
    set (extra_macro_args ${ARGN})
    if (extra_macro_args)
        useQtModules(${extra_macro_args} TARGET ${TARGET})

        get_property(_qt_libs GLOBAL PROPERTY qt_libs)
        set(_qt_libs "${_qt_libs}" ${extra_macro_args})
        list(REMOVE_DUPLICATES _qt_libs)
        set_property(GLOBAL PROPERTY qt_libs "${_qt_libs}")
    endif()
endmacro()

# Link Boost modules to target
macro(linkBoost TARGET)
    set (extra_macro_args ${ARGN})
    if (extra_macro_args)
        useBoost(${extra_macro_args} TARGET ${TARGET})

        get_property(_boost_libs GLOBAL PROPERTY boost_libs)
        set(_boost_libs "${_boost_libs}" ${extra_macro_args})
        list(REMOVE_DUPLICATES _boost_libs)
        set_property(GLOBAL PROPERTY boost_libs "${_boost_libs}")
    endif()
endmacro()

# Link libraries
macro(linkLibs TARGET)
    set (extra_macro_args ${ARGN})
    if (extra_macro_args)
        get_target_property(TYPE ${TARGET} TYPE)
        if (TYPE STREQUAL INTERFACE_LIBRARY)
            target_link_libraries(${TARGET} INTERFACE ${extra_macro_args})
        else()
            target_link_libraries(${TARGET} ${extra_macro_args})
        endif()
    endif()
endmacro()

# Add binary target (either shared library or executable)
# Options:
# TARGET - target name
# SOURCES - list of source files to include to build
# GLOBBING - regexp for parsing sources recursively
# EXCEPTS - do not include files
# LIBS - list of libraries to link to target
# QT - list of Qt modules to add to target
# BOOST - list of boost modules to add to target
# TYPE - target type (shared executable) - mandatory
macro(binaryTarget TARGET TYPE)
    set(oneValueArgs NOINSTALL COMPONENT RECURSIVE SUBDIR)
    set(multiValueArgs SOURCES LIBS QT BOOST GLOBBING DEPENDS)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (OPTIONS_SUBDIR)
        set(PLUGIN_PATH ${PLUGIN_PATH}/${OPTIONS_SUBDIR})
    endif()

    message(STATUS "Creating ${TYPE}: ${TARGET}")
    if (OPTIONS_SOURCES)
        message(STATUS "Sources: ${OPTIONS_SOURCES}")
    endif()
    message(STATUS "Link libraries: ${OPTIONS_LIBS}")
    message(STATUS "Using Qt modules: ${OPTIONS_QT}")
    message(STATUS "Using boost modules: ${OPTIONS_BOOST}")
    message(STATUS "Globing: ${OPTIONS_GLOBBING}")
    message(STATUS "Using install path: ${LIBRARY_PATH}")
    message(STATUS "Using plugin path: ${PLUGIN_PATH}")
    message(STATUS "Depends of: ${OPTIONS_DEPENDS}")

    # If component not set explicitely, then
    # - try package-aware COMPONENT_NAME, if it is not set
    # - use TARGET instead
    if (NOT OPTIONS_NOINSTALL AND NOT OPTIONS_COMPONENT)
        message(FATAL_ERROR "No COMPONENT is set for target ${TARGET}")
    endif()
    debug("Component: ${OPTIONS_COMPONENT}")

    set(_srcInternal "")
    if (OPTIONS_GLOBBING)
        sources(
            _srcInternal
            GLOBBING ${OPTIONS_GLOBBING}
            EXCEPT ${OPTIONS_EXCEPT}
            RECURSIVE ${OPTIONS_RECURSIVE}
        )
    endif()

    string(TOUPPER ${TYPE} TYPE_)
    if (TYPE_ STREQUAL "SHARED")
        add_library(${TARGET} SHARED ${OPTIONS_SOURCES} ${_srcInternal})
    elseif (TYPE_ STREQUAL "STATIC")
        if (NOT ${TARGET}_install_headers)
            set(OPTIONS_NOINSTALL true)
        endif()
        add_library(${TARGET} STATIC ${OPTIONS_SOURCES} ${_srcInternal})
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    elseif (TYPE_ STREQUAL "INTERFACE")
        # Emulate INTERFACE behaviour for older cmake versions
        set(OPTIONS_NOINSTALL true)
        add_library(${TARGET} STATIC ${OPTIONS_SOURCES} ${_srcInternal})
    elseif(TYPE_ STREQUAL "EXECUTABLE")
        if (WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL Debug)
            set(HIDE_CMD_WINDOW_PLATFORM "WIN32")
        else ()
            set(HIDE_CMD_WINDOW_PLATFORM "")
        endif ()
        add_executable(${TARGET} ${HIDE_CMD_WINDOW_PLATFORM} ${OPTIONS_SOURCES} ${_srcInternal})
    elseif(TYPE_ STREQUAL "PLUGIN")
        set(OPTIONS_QT Core ${OPTIONS_QT})
        add_library(${TARGET} SHARED ${OPTIONS_SOURCES} ${_srcInternal})
    endif()

    set(TARGET_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/_includes")
    if (NOT TYPE_ STREQUAL "INTERFACE")
        target_include_directories(${TARGET} PRIVATE "${TARGET_INCLUDE_PATH}")
    endif()
    set_target_properties(${TARGET} PROPERTIES
        "INTERFACE_INCLUDE_DIRECTORIES" "${TARGET_INCLUDE_PATH}")

    if (NOT OPTIONS_NOINSTALL)
        # Merge component dependencies
        if(NOT TARGET ${OPTIONS_COMPONENT})
            add_custom_target(${OPTIONS_COMPONENT})
        endif()

        get_property(component_deps TARGET ${OPTIONS_COMPONENT} PROPERTY DEPENDS_OF)
        set(_all_component_dependencies "${OPTIONS_DEPENDS}" ${component_deps})
        foreach(component ${OPTIONS_DEPENDS})
            if (TARGET ${component})
                get_property(_deps TARGET ${component} PROPERTY DEPENDS_OF)
                if(_deps)
                    set(_all_component_dependencies "${_all_component_dependencies}" "${_deps}")
                endif()
            endif()
        endforeach()

        list(REMOVE_DUPLICATES _all_component_dependencies)
        set_property(TARGET ${OPTIONS_COMPONENT} PROPERTY DEPENDS_OF ${_all_component_dependencies})
    endif()

    if (${TARGET}_install_headers)
        install(DIRECTORY ${TARGET_INCLUDE_PATH}/
                DESTINATION ${INCLUDE_PATH}
                COMPONENT ${OPTIONS_COMPONENT}-dev)
    endif()

    linkQt(${TARGET} ${OPTIONS_QT})
    linkBoost(${TARGET} ${OPTIONS_BOOST})
    linkLibs(${TARGET} ${OPTIONS_LIBS} ${OPTIONS_DEPENDS})

    if (NOT OPTIONS_NOINSTALL)
        installTarget(${TARGET} ${TYPE_} ${OPTIONS_COMPONENT})
    endif()
endmacro()

macro(installTarget TARGET TYPE COMPONENT)
    if(${TYPE} STREQUAL "PLUGIN")
        # If destination path is relative we should put plugins to directory related to binary path
        convertRelativePath(${PLUGIN_PATH} INSTALL_PLUGIN_PATH)
        debug("Install plugin [${PROJECT_NAME}] to " ${INSTALL_PLUGIN_PATH})

        install(TARGETS ${TARGET}
            LIBRARY DESTINATION ${INSTALL_PLUGIN_PATH} OPTIONAL
            COMPONENT "${COMPONENT}"
            RUNTIME DESTINATION ${INSTALL_PLUGIN_PATH} OPTIONAL
            COMPONENT "${COMPONENT}")
    else()
        if (NOT DEFINED BINARY_PATH OR NOT DEFINED LIBRARY_PATH)
            message(FATAL_ERROR "BINARY_PATH or LIBRARY_PATH is empty, you must "
                                "define these paths first")
        endif()

        install(TARGETS ${TARGET}
          RUNTIME DESTINATION ${BINARY_PATH} OPTIONAL
          COMPONENT "${COMPONENT}"
          LIBRARY DESTINATION ${LIBRARY_PATH} OPTIONAL
          COMPONENT "${COMPONENT}"
          ARCHIVE DESTINATION ${LIBRARY_PATH} OPTIONAL
          COMPONENT "${COMPONENT}")
    endif()
endmacro()

macro (libraryTarget TARGET TYPE)
    set(oneValueArgs COMPONENT NOINSTALL)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "" ${ARGN})
    if (NOT OPTIONS_NOINSTALL)
        if (NOT OPTIONS_COMPONENT)
            set(OPTIONS_COMPONENT ${TARGET})
        endif()
        binaryTarget(${TARGET} ${TYPE} COMPONENT ${OPTIONS_COMPONENT} ${ARGN})
    else()
        binaryTarget(${TARGET} ${TYPE} ${ARGN})
    endif()
endmacro()

# Create static library target
macro(staticLib TARGET)
    libraryTarget(${TARGET} STATIC ${ARGN})
endmacro(staticLib)

# Create shared library target
macro(sharedLib TARGET)
    libraryTarget(${TARGET} SHARED ${ARGN})
endmacro(sharedLib)

# Create executable target
macro(executable TARGET)
    set(oneValueArgs COMPONENT NOINSTALL)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "" ${ARGN})
    if (NOT OPTIONS_NOINSTALL)
        if (NOT OPTIONS_COMPONENT)
            if(COMPONENT_NAME)
                set(OPTIONS_COMPONENT ${COMPONENT_NAME})
            else()
                message(FATAL_ERROR "No COMPONENT or COMPONENT_NAME is set for target ${TARGET}")
            endif()
        endif()
        binaryTarget(${TARGET} EXECUTABLE COMPONENT ${OPTIONS_COMPONENT} ${ARGN})
    else()
        binaryTarget(${TARGET} EXECUTABLE COMPONENT ${ARGN})
    endif()
endmacro(executable)

macro(qtPlugin TARGET)
    add_definitions(-DQT_PLUGIN)

    if (NOT DEFINED PLUGIN_PATH)
        message(FATAL_ERROR "PLUGIN_PATH is empty, you must define it first")
    endif()

    binaryTarget(${TARGET} PLUGIN ${ARGN})
endmacro()


# Add custom target for headers. No compilation here just for IDE
# Options:
# SOURCES - list of source files to include
macro(headerOnly)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(OPTIONS "" "" "${multiValueArgs}" ${ARGN})

    string(RANDOM _target)
    debug("Headers only target: ${OPTIONS_SOURCES}")
    add_custom_target(${_target} SOURCES ${OPTIONS_SOURCES})
endmacro(headerOnly)

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

# Define root path for TARGET
macro(defineModuleRoot TARGET)
    string(TOUPPER ${TARGET} TARGET_)
    parentProperty(${TARGET_}_PATH ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

# Set option for root project
macro(setOption option value)
    if (IS_ROOT)
        set(${option} ${value} CACHE BOOL "Set ${option} to OFF" FORCE)
    endif()
endmacro()

option(BUILD_EXAMPLES "Build project examples" OFF)
# Add examples macro
macro(examples PATH)
    if (IS_ROOT OR BUILD_EXAMPLES)
        add_subdirectory(${PATH})
    endif()
endmacro()

option(TRANSLATE_RECURSIVE "Translate all submodules while updating translations" OFF)
# Use this macro in the begining of CMakeLists.txt to define item
macro(item TARGET)
    project(${TARGET})
    message(STATUS "=======================  ${TARGET}  =======================")

    add_custom_target(_item_${TARGET}_guard)

    if (NOT DEFINED ${TARGET}_build_type)
        set(${TARGET}_build_type STATIC)
    endif()

    if (${IS_ROOT} OR ${TRANSLATE_RECURSIVE})
        set(SHOULD_TRANSLATE TRUE)
    else()
        set(SHOULD_TRANSLATE FALSE)
    endif()

    if (NOT TRANSLATION_DOMAIN)
        set (TRANSLATION_DOMAIN ${TARGET})
    endif()

    i18n(${TARGET} "ru" ${CMAKE_CURRENT_SOURCE_DIR} ${SHOULD_TRANSLATE})
endmacro()

# The same with `item` but can be disabled via _use_<item> option
macro(optional_item TARGET ENABLED_BY_DEFAULT)
    option(_use_${TARGET} "Include item: ${TARGET}" ${ENABLED_BY_DEFAULT})

    if (NOT _use_${TARGET})
        return()
    endif()
    item(${TARGET})
endmacro()

# One can use this macro to add an item defined by `item` macro
macro(add_item TARGET PATH)
    if (NOT TARGET _item_${TARGET}_guard)
        set(oneValueArgs SHARED HEADERS_TARGET)
        cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "" ${ARGN})

        set(_use_${TARGET} ON)

        if (${OPTIONS_SHARED})
            set(${TARGET}_build_type SHARED)
        else()
            set(${TARGET}_build_type STATIC)
        endif()

        if (${OPTIONS_HEADERS_TARGET})
            set(${TARGET}_install_headers TRUE)
        endif()

        add_subdirectory(${PATH} ${CMAKE_CURRENT_BINARY_DIR}/items/${TARGET})

        # clear all variables
        unset(${item_name}_build_type)
        unset(${item_name}_install_headers)
    endif()
endmacro()

# One should use this macro to format external dependencies (either boost or qt to
# CPACK_DEBIAN_PACKAGE_DEPENDS)
# output is var where output should be put in
# targetName is the name of global property to discover lib names
# formatString is a format where lib name should be put (use % placeholder to define place fo lib name)
# do not include lib if it in exlude list
macro (formatLibsDependencies output targetName formatString)
    set(multiValueArgs INCLUDE EXCLUDE)
    cmake_parse_arguments(OPTIONS "" "" "${multiValueArgs}" ${ARGN})

    get_property(_libNames GLOBAL PROPERTY ${targetName})

    foreach(_lib ${_libNames} ${OPTIONS_INCLUDE})
        list (FIND OPTIONS_EXCLUDE "${_lib}" _index)
        if (NOT ${_index} GREATER -1)
            string(TOLOWER ${_lib} _libName)
            string(REPLACE "%" "${_libName}" _formatted "${formatString}")
            list(APPEND ${output} "${_formatted}")
        endif()
    endforeach()
endmacro()
