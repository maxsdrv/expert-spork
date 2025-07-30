project(plugins_test)

set(PLUGIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/plugins)
add_definitions(-DPLUGIN_PATH="${PLUGIN_PATH}")

qtest(${PROJECT_NAME}
    GLOBBING *.*
    LIBS mpkplugins
    GMOCK true)

useQtModules(Test)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_PATH})

add_subdirectory(service)
add_subdirectory(nonservice)
