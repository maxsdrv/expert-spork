option(BUILD_TESTS "Build tests" ON)

macro(tests SUBDIR)
    if (BUILD_TESTS)
        add_subdirectory(${SUBDIR} ${ARGN})
    endif()
endmacro()

if (NOT DEFINED QT_QMAKE_EXECUTABLE)
    set(QT_QMAKE_EXECUTABLE qmake)
endif()

execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_BINS OUTPUT_VARIABLE QT_BINARIES_PATH)
find_program(MOC_EXECUTABLE qt_moc moc PATHS ${QT_BINARIES_PATH})


# Add test executable
# Options:
# TARGET - test name (mandatory)
# SOURCES - list of source files to include to build
# GLOBBING - regexp for parsing sources recursively
# LIBS - list of libraries to link to target
# GMOCK - should we use Google Mock Library for this test
macro(gtest TARGET)
    function(testSources SRC)
        set(oneValueArgs RECURSIVE)
        set(multiValueArgs GLOBBING EXCEPT)
        cmake_parse_arguments(COLLECT_OPTIONS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        if("${COLLECT_OPTIONS_GLOBBING}" STREQUAL "")
            set(COLLECT_OPTIONS_GLOBBING "*.cpp" "*.h" "*.ui" "*.qrc")
        endif()

        if("${COLLECT_OPTIONS_RECURSIVE}" STREQUAL "true")
            file(GLOB_RECURSE _globed ${COLLECT_OPTIONS_GLOBBING})
        else()
            file(GLOB _globed ${COLLECT_OPTIONS_GLOBBING})
        endif()

        if(COLLECT_OPTIONS_EXCEPT)
            foreach(exception ${COLLECT_OPTIONS_EXCEPT})
                foreach(item ${_globed})
                    string(REGEX MATCH ${exception} item ${item})
                    if(item)
                        list(REMOVE_ITEM _globed ${item})
                        continue()
                    endif(item)
                endforeach()
            endforeach()
        endif()
        set(${SRC} ${_globed} PARENT_SCOPE)
    endfunction()


    set(oneValueArgs RECURSIVE GMOCK)
    set(multiValueArgs SOURCES LIBS GLOBBING)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_srcInternal "")
    if (OPTIONS_GLOBBING)
        testSources(
            _srcInternal
            GLOBBING ${OPTIONS_GLOBBING}
            RECURSIVE ${OPTIONS_RECURSIVE}
        )
    endif()

    add_executable(${TARGET} ${OPTIONS_SOURCES} ${_srcInternal})

    if (OPTIONS_GMOCK)
        set(TEST_LIBRARIES gtest gmock_main gmock)
    else()
        set(TEST_LIBRARIES gtest gtest_main)
    endif()
    target_link_libraries(${TARGET}
        ${TEST_LIBRARIES}
        ${OPTIONS_LIBS})

    add_dependencies(${TARGET} gtest)

    # Tell ctest about tests
    include(GoogleTest)
    gtest_add_tests(
        TARGET      ${TARGET}
        AUTO)
endmacro()

macro(qtest TARGET)
    set(oneValueArgs RECURSIVE GMOCK)
    set(multiValueArgs QT SOURCES LIBS GLOBBING)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    gtest(${TARGET} RECURSIVE ${OPTIONS_RECURSIVE}
        GMOCK ${OPTIONS_GMOCK}
        SOURCES ${OPTIONS_SOURCES}
        LIBS ${OPTIONS_LIBS}
        GLOBBING ${OPTIONS_GLOBBING}
        ${OPTIONS_UNPARSED_ARGUMENTS})
    target_link_libraries(${TARGET} gqtest)

    if (OPTIONS_QT)
        set_property(TARGET Qt5::moc PROPERTY IMPORTED_LOCATION ${MOC_EXECUTABLE})
        find_package(${QT_NAME} COMPONENTS ${OPTIONS_QT} REQUIRED)

        foreach(module ${OPTIONS_QT})
            target_link_libraries(${TARGET} ${QT_NAME}::${module})
        endforeach(module)
    endif()
endmacro()


enable_testing()
