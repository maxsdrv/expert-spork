if (_useBoost)
    return()
endif()
set(_useBoost true)

# Boost is spawning pragma warnings since 1.70 about global bind
# placeholders even for internal libraries
# This makes it silent
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

include(CMakeParseArguments)

#If you want to redefine Boost paths use the following hints
# BOOST_ROOT             - Preferred installation prefix
# BOOST_INCLUDEDIR       - Preferred include directory e.g. <prefix>/include
# BOOST_LIBRARYDIR       - Preferred library directory e.g. <prefix>/lib
# Boost_NO_SYSTEM_PATHS  - Set to ON to disable searching in locations not
#                          specified by these hint variables. Default is OFF.
function(useBoost)
    cmake_parse_arguments(USEBOOST "" "TARGET" "" ${ARGN})

    if("${USEBOOST_TARGET}" STREQUAL "")
        set(USEBOOST_TARGET ${PROJECT_NAME})
    endif()

    find_package(Boost REQUIRED COMPONENTS  ${USEBOOST_UNPARSED_ARGUMENTS})
    target_include_directories(${USEBOOST_TARGET} PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${USEBOOST_TARGET} ${Boost_LIBRARIES})
endfunction(useBoost)
