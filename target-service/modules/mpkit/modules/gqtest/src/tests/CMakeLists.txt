project(gqtest_tests)

qtest(${PROJECT_NAME} GLOBBING "*" GMOCK true)
