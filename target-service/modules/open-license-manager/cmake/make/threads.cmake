# @file
# @brief This file add some system components
#

function(threads TARGET)
    find_package(Threads REQUIRED) 
    target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
endfunction()
