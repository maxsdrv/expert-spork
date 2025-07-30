if(__cpack_guard)
    return()
endif()
set(__cpack_guard true)

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
    include (${CMAKE_SCRIPTS_PATH}/pack/rpm.cmake)
    include (${CMAKE_SCRIPTS_PATH}/pack/deb.cmake)
    include (${CMAKE_SCRIPTS_PATH}/pack/nsis.cmake)
    include (${CMAKE_SCRIPTS_PATH}/pack/tgz.cmake)

    include (${CMAKE_SCRIPTS_PATH}/pack/cpack.cmake)

    # Find cpack executable
    find_program(CPACK_EXECUTABLE NAMES cpack)
    set(CPACK_FOUND true)
else()
    message(WARNING "No Cpack found")
endif()
