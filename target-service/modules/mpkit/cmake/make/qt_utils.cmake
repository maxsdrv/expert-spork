function(useQtModule target module)
    find_package(${QT_NAME} COMPONENTS ${module} REQUIRED)
    get_target_property(TYPE ${target} TYPE)
    if (TYPE STREQUAL INTERFACE_LIBRARY)
        target_link_libraries(${target} INTERFACE ${QT_NAME}::${module})
    else()
        target_link_libraries(${target} ${QT_NAME}::${module})
    endif()
endfunction(useQtModule)

# on old OS (like astra) doesn't sets some variales for qt5 executables
# but qt4 variables can be used
macro(setupQtLocationVariable TARGET VARIABLE DEFAULT_VALUE HELP_STRING)
    set(EXECUTABLE ${DEFAULT_VALUE})

    if (TARGET ${TARGET})
        get_target_property(TARGET_EXECUTABLE ${TARGET} IMPORTED_LOCATION)

        if (TARGET_EXECUTABLE)
            set(EXECUTABLE ${TARGET_EXECUTABLE})
        endif()
    endif()

    set(${VARIABLE} "${EXECUTABLE}" CACHE STRING ${HELP_STRING} FORCE)
endmacro()

function(setQtBinaryVar VARIABLE PROGRAM PATH HELP_STRING)
    find_program(${VARIABLE} ${PROGRAM} PATHS ${PATH})

    # The target has to be created here in order to make CMake able find Qt
    # autotools by the correct (native OS) paths during a cross-compilation.
    # Qt autotools uses Qt5:<autotool_name> targets which must be declared
    # before any Qt package will be included in CMakeList.txt file.
    if (NOT TARGET Qt5::${PROGRAM})
        add_executable(Qt5::${PROGRAM} IMPORTED)
    endif()

    if (TARGET Qt5::${PROGRAM})
         set_property(TARGET Qt5::${PROGRAM} PROPERTY IMPORTED_LOCATION ${${VARIABLE}})
    endif()

    message(STATUS "${HELP_STRING} is ${${VARIABLE}}")
endfunction()

function(useQtModules)
    cmake_parse_arguments(USEQT "" "TARGET" "" ${ARGN})

    if("${USEQT_TARGET}" STREQUAL "")
        set(USEQT_TARGET ${PROJECT_NAME})
    endif()

    foreach(module ${USEQT_UNPARSED_ARGUMENTS})
        useQtModule(${USEQT_TARGET} ${module})
    endforeach(module)
endfunction(useQtModules)
