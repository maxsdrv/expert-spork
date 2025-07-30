project(mpkutils_test)

set(AUX_PATH ${CMAKE_CURRENT_BINARY_DIR}/aux_app)
add_definitions(-DAUX_PATH="${AUX_PATH}")

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    LIBS mpkutils)

useBoost(filesystem regex system thread)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AUX_PATH})

add_subdirectory(aux_app)
