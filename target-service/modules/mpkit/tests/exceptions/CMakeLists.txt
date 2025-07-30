project(mpkexceptions_test)

gtest(${PROJECT_NAME}
    GLOBBING "*"
    LIBS mpkexceptions)
