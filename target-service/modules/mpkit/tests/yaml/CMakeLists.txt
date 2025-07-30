project(yaml_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core
    LIBS mpkyaml)

linkBoost(${PROJECT_NAME} system)
