project(json-parser)

set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
generatePaths(json SRC)

file(GLOB_RECURSE SOURCES "*.*")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    mpkjson)
