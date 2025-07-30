project(httpws_test)

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    set(STDC_FS stdc++fs)
endif()

qtest(
    ${PROJECT_NAME}
    GLOBBING "*.cpp" "*.h"
    RECURSIVE false
    QT Core Test
    LIBS mpkhttpws mpkutils ${STDC_FS}
    GMOCK true)

set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
generatePathsHeader(${PROJECT_NAME} PREFIX appkit)

file(GLOB _files "*.json")
add_custom_target(${PROJECT_NAME}_config SOURCES ${_files})
