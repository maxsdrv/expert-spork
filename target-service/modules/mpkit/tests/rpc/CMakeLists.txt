project(rpc_test)

qtest(${PROJECT_NAME}
    GLOBBING "*.*"
    RECURSIVE false
    QT Core
    LIBS mpkrpc
    GMOCK true)

set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
generatePathsHeader(${PROJECT_NAME} PREFIX appkit)

target_include_directories(${PROJECT_NAME} PRIVATE ${RPC_PRIVATE_HEADERS_PATH})
