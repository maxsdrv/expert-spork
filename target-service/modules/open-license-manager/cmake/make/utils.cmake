# Some utility mocros

option(VERBOSE_CMAKE "More verbose output in Cmake processing" OFF)

# Display message if VERBOSE_CMAKE in ON
macro(debug)
    if (VERBOSE_CMAKE)
        message(${ARGN})
    endif()
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
