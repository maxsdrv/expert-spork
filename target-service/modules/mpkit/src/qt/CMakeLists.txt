optional_item(mpkqt OFF)

file(GLOB_RECURSE lib_sources "*.*")

list(APPEND lib_deps mpkexceptions mpkqttypes)
if (NOT APPLE)
   list(APPEND lib_deps rt)
endif()

if (NOT UNIX)
    list(TRANSFORM  lib_sources REPLACE ".*UnixSignalHandler.*" "")
    list(REMOVE_ITEM lib_deps "rt")
endif()

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING ${lib_sources}
    RECURSIVE true
    QT Core
    DEPENDS ${lib_deps}
    COMPONENT ${PROJECT_NAME})
