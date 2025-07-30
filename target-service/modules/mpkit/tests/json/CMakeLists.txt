project(json_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core
    LIBS mpkjson)

linkBoost(${PROJECT_NAME} system)
