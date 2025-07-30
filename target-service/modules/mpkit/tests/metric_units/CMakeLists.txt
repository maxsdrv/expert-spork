project(metric_units_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core
    LIBS mpkmetric_units mpkexceptions mpkutils
    GMOCK true)

