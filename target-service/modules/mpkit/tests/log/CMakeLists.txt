project(mpklog_test)

gtest(${PROJECT_NAME}
    GLOBBING "*.cpp" "*.h"
    LIBS mpklog)

set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
generatePathsHeader(${PROJECT_NAME} PREFIX appkit)

file(GLOB _files "*.ini")
add_custom_target(${PROJECT_NAME}_config SOURCES ${_files})
