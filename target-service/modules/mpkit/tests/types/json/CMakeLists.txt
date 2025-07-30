project(types_json_test)

gtest(${PROJECT_NAME}
    GLOBBING "*.*"
    LIBS mpktypes mpkjson)

