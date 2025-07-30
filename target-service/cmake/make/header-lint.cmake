# @file
# @brief Treat C++ headers as sources for clang-tidy linting

# Include this file before the first 'project' statement:
# cmake_minimum_required(VERSION X.Y)
# include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/make/header-lint.cmake)
# project(some_project)

if (CODE_LINT)
    message("Code lint is ON")

    function (treatHeadersAsSource variable access value current stack)
        if (${access} STREQUAL "MODIFIED_ACCESS")
            set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;m;mm;mpp;CPP;ixx;cppm;h;hpp;HPP;H PARENT_SCOPE)
            set(CMAKE_CXX_IGNORE_EXTENSIONS inl;o;O;obj;OBJ;def;DEF;rc;RC PARENT_SCOPE)
        endif()
    endfunction()

    variable_watch(CMAKE_CXX_SOURCE_FILE_EXTENSIONS treatHeadersAsSource)
endif()
