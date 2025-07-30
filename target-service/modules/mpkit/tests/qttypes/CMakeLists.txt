project(mpkqt_test)

gtest(${PROJECT_NAME}
    GLOBBING "*"
    LIBS mpkqttypes)
