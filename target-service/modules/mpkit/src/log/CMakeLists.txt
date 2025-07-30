optional_item(mpklog OFF)

# This definition will be auto included with addModule function
set(Boost_USE_MULTITHREADED ON)
add_definitions(-DBOOST_LOG_DYN_LINK)

libraryTarget(${PROJECT_NAME}
    ${${PROJECT_NAME}_build_type}
    GLOBBING "*"
    RECURSIVE true
    BOOST log log_setup system thread
    THREADS
    COMPONENT ${PROJECT_NAME})

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} BASE_NAME ${PROJECT_NAME}
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}_export.h)

tests(${MPKIT_PATH}/tests/log ${CMAKE_CURRENT_BINARY_DIR}/tests)
