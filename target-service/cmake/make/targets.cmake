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
# TARGET - target name - mandatory
# SOURCES - list of explicit source files for the target build
# GLOBBING - regexp for collecting sources for the target build
# RECURSIVE - true/false for recursive globbing
# EXCEPT - regexp for remove files from globbing
# LIBS - list of libraries to link to target (also catches its public headers)
# DEPENDS - same as LIBS but also populate dependency property used in cpack
# QT - list of Qt modules to add to target
# BOOST - list of boost modules to add to target
# TYPE - target type (shared static interface executable) - mandatory
# SUBDIR - subdirectory for plugins
# NOINSTALL - target not meant to be installed
# THREADS - link with system threads library
# COMPONENT - installation component (mandatory if not NOINSTALL)
macro(binaryTarget TARGET TYPE)
    set(optArgs THREADS)
    set(oneValueArgs NOINSTALL COMPONENT RECURSIVE SUBDIR)
    set(multiValueArgs SOURCES LIBS QT BOOST GLOBBING EXCEPT DEPENDS)
    cmake_parse_arguments(OPTIONS "${optArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "Creating ${TYPE}: ${TARGET}")
    if (OPTIONS_LIBS OR OPTIONS_DEPENDS)
        printMessage(STATUS "Link libraries:")
        if (OPTIONS_LIBS)
            printList(STATUS "${OPTIONS_LIBS}" INDENT)
        endif()
        if (OPTIONS_DEPENDS)
            printList(STATUS "${OPTIONS_DEPENDS}" INDENT)
        endif()
    endif()
    if (OPTIONS_QT)
        printList(STATUS "Using Qt modules:")
        printList(STATUS "${OPTIONS_QT}" INDENT)
    endif()
    if(OPTIONS_BOOST)
        printList(STATUS "Using boost modules:")
        printList(STATUS "${OPTIONS_BOOST}" INDENT)
    endif()

    printMessage(STATUS "Using binary path: ${BINARY_PATH}")
    printMessage(STATUS "Using library path: ${LIBRARY_PATH}")

    if (OPTIONS_SUBDIR)
        set(PLUGIN_PATH ${PLUGIN_PATH}/${OPTIONS_SUBDIR})
    endif()
    printMessage(STATUS "Using plugin path: ${PLUGIN_PATH}")

    if (OPTIONS_SOURCES)
        printMessage(VERBOSE "Explicit sources:")
        printList(VERBOSE "${OPTIONS_SOURCES}" INDENT)
    endif()

    set(_srcInternal "")
    if (OPTIONS_GLOBBING)
        printMessage(STATUS "Globing: ${OPTIONS_GLOBBING}")
        if (OPTIONS_EXCEPT)
            printMessage(STATUS "Except: ${OPTIONS_EXCEPT}")
        endif()
        if (OPTIONS_RECURSIVE)
            printMessage(STATUS "Recursive: ${OPTIONS_RECURSIVE}")
        else()
            printMessage(STATUS "Recursive: false")
        endif()

        sources(
            _srcInternal
            GLOBBING ${OPTIONS_GLOBBING}
            EXCEPT ${OPTIONS_EXCEPT}
            RECURSIVE ${OPTIONS_RECURSIVE})

        if(_srcInternal)
            printMessage(VERBOSE "Globed sources:")
            printList(VERBOSE "${_srcInternal}" INDENT)
        endif()
    endif()

    # If component not set explicitely, then
    # - try package-aware COMPONENT_NAME, if it is not set
    # - use TARGET instead
    if (NOT OPTIONS_NOINSTALL AND NOT OPTIONS_COMPONENT)
        message(FATAL_ERROR "No COMPONENT is set for target ${TARGET}")
    endif()
    if(OPTIONS_COMPONENT)
        printMessage(VERBOSE "Component: ${OPTIONS_COMPONENT}")
    else()
        printMessage(VERBOSE "Component: not set (but it's ok)")
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
    target_include_directories(${TARGET} PUBLIC "${TARGET_INCLUDE_PATH}")

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

    if(OPTIONS_THREADS)
        find_package(Threads REQUIRED)
        target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
    endif()

    if (NOT OPTIONS_NOINSTALL)
        installTarget(${TARGET} ${TYPE_} ${OPTIONS_COMPONENT})
    endif()
endmacro()

macro(installTarget TARGET TYPE COMPONENT)
    if(${TYPE} STREQUAL "PLUGIN")
        # If destination path is relative we should put plugins to directory related to binary path
        convertRelativePath(${PLUGIN_PATH} INSTALL_PLUGIN_PATH)
        printMessage(VERBOSE "Install plugin [${PROJECT_NAME}] to " ${INSTALL_PLUGIN_PATH})

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
    printMessage(VERBOSE "Headers only target: ${OPTIONS_SOURCES}")
    add_custom_target(${_target} SOURCES ${OPTIONS_SOURCES})
endmacro(headerOnly)
