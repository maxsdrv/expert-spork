project(types_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    LIBS mpktypes)

if (_use_mpkjson)
    add_subdirectory(json)
endif()
