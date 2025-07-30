project(storage_adapter_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core
    LIBS mpkstorage_adapter
    GMOCK true)
