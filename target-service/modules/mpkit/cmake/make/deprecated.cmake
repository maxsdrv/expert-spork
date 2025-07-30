#This function provide includePath for module and adds custom target with includes
function(addIncludes)
    message(DEPRECATION "addIncludes() is deprecated and will be removed in the near future")
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
    message(DEPRECATION "defineProjectIncludes() is deprecated and will be removed in the near future")
    # Include generated includes directory
    set(PROJECT_GENERATED_INCLUDES ${PATH}/include)
    include_directories(src ${PROJECT_GENERATED_INCLUDES})

    addIncludes(${CMAKE_CURRENT_SOURCE_DIR}/include # source include directory
        ${PROJECT_GENERATED_INCLUDES}               # generated include directory
        ${ARGN})                                    # optional include directories
endmacro(defineProjectIncludes)

macro(generatePaths PATH SRC)
    message(DEPRECATION "generatePaths() is deprecated and will be removed in the near future")
    preparePaths(${PROJECT_NAME})
    configure_file(${CMAKE_SCRIPTS_PATH}/templates/paths.h.in ${PATH}/Paths.h @ONLY)
endmacro()

function(threads TARGET)
    message(DEPRECATION "threads() is deprecated and will be removed in the near future")
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
endfunction()
