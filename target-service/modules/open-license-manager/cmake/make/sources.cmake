# @file
# @brief Helper functions for adding files to targets
#

# Find project files with GLOBBING expression
# Default GLOBBING "*.cpp *.h *.ui *.qrc"
function(sources SRC)
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


#This function provide includePath for module and adds custom target with includes
function(addIncludes)
    set(includePath ${includePath} ${ARGN} PARENT_SCOPE)
    foreach(path ${ARGN})
        file(GLOB_RECURSE _include_files "${path}/*/*")
        list(APPEND _include ${_include_files})
    endforeach()
    add_custom_target(${PROJECT_NAME}_includes SOURCES ${_include})
    include_directories(${ARGN})
endfunction(addIncludes)

# Define per project generated include path and default include path
macro(defineProjectIncludes PATH)
    # Include generated includes directory
    set(PROJECT_GENERATED_INCLUDES ${PATH}/include)
    include_directories(src ${PROJECT_GENERATED_INCLUDES})

    addIncludes(${CMAKE_CURRENT_SOURCE_DIR}/include # source include directory
        ${PROJECT_GENERATED_INCLUDES}               # generated include directory
        ${ARGN})                                    # optional include directories
endmacro(defineProjectIncludes)

# Include in-project *.cmake file.
# This macro works together with addModule macro.
# If project is top-level one then FILE will be included, otherwise FILE will be added to ${TARGET}_CMAKE variable
# which in turn will be parsed in addModule
macro(includeCmake FILE)
    include(${FILE})
    if (NOT IS_ROOT)
        set(${PROJECT_NAME}_CMAKE ${${PROJECT_NAME}_CMAKE} ${FILE})
    endif()
endmacro()

macro(commonSources TARGET SRC)
    generateDomains(${PROJECT_GENERATED_INCLUDES}/${TARGET} SRC "${TRANSLATION_DOMAINS}")
    generatePaths(${PROJECT_GENERATED_INCLUDES}/${TARGET} SRC)
endmacro()
