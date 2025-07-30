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
