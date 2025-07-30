project(nonservice)

qtPlugin(${PROJECT_NAME}
    GLOBBING "*"
    QT Core
    NOINSTALL true)

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:nonservice>"
        "$<TARGET_FILE_DIR:nonservice>/copy_$<TARGET_FILE_NAME:nonservice>")
