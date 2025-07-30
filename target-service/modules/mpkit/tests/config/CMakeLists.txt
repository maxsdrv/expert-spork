project(mpkconfig_test)

gtest(${PROJECT_NAME}
    GLOBBING "*.cpp" "*.h"
    RECURSIVE true
    LIBS mpkconfig
    GMOCK true)

set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
generatePathsHeader(${PROJECT_NAME} PREFIX appkit)

file(GLOB _files "*.json")
add_custom_target(${PROJECT_NAME}_config SOURCES ${_files})
