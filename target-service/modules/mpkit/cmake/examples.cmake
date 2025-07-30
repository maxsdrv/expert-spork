option(BUILD_EXAMPLES "Build project examples" OFF)

if(__examples_guard)
    return()
endif()
set(__examples_guard true)

if(NOT EXAMPLES_PATH)
    set(EXAMPLES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/examples")
endif()

# Add examples macro
macro(examples)
    if (IS_ROOT AND BUILD_EXAMPLES)
        add_subdirectory(${EXAMPLES_PATH})
    endif()
endmacro()
