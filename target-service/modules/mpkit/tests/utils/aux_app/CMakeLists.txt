project(app_test_aux)

executable(
    ${PROJECT_NAME}
    GLOBBING "*"
    RECURSIVE false
    LIBS mpkapp pthread
    BOOST thread system log
    NOINSTALL true)
