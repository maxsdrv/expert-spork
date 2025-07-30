# Find project files with GLOBBING expression
# Default GLOBBING "*.cpp *.h *.ui *.qrc"
function(sources SRC)
    set(oneValueArgs RECURSIVE)
    set(multiValueArgs GLOBBING EXCEPT)
    cmake_parse_arguments(OPTIONS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if("${OPTIONS_GLOBBING}" STREQUAL "")
        set(OPTIONS_GLOBBING "*.cpp" "*.h" "*.ui" "*.qrc")
    endif()

    if("${OPTIONS_RECURSIVE}" STREQUAL "true")
        file(GLOB_RECURSE _globed ${OPTIONS_GLOBBING})
    else()
        file(GLOB _globed ${OPTIONS_GLOBBING})
    endif()

    if(OPTIONS_EXCEPT)
        foreach(exception ${OPTIONS_EXCEPT})
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

function(scanProject TARGET)
    sources(FILES ${ARGN})
    add_custom_target(${TARGET}_files SOURCES ${FILES})
endfunction()

function(scanHeaders TARGET PATH)
    scanProject(${TARGET} GLOBBING "${PATH}/*.h" "${PATH}/*.hpp" RECURSIVE true)
endfunction()
