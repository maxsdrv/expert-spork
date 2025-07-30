project(gqtest)

file(GLOB _src "*.*")

find_package(
    ${QT_NAME}
    COMPONENTS Core
    REQUIRED)
set(CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME} STATIC ${_src})

target_link_libraries(${PROJECT_NAME} PUBLIC gtest ${QT_NAME}::Core)

set_target_properties(${PROJECT_NAME} PROPERTIES "INTERFACE_INCLUDE_DIRECTORIES"
                                                 "${CMAKE_CURRENT_SOURCE_DIR}")

tests(tests)
