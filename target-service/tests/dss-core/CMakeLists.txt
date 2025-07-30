project(dss-core_test)

generatePaths(${PROJECT_GENERATED_INCLUDES}/${PROJECT_NAME} DUMMY)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core Test
    LIBS dss-core
    GMOCK true)

