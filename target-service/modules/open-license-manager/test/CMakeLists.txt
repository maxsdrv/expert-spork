#if we're here boost has been found
add_definitions(-DBOOST_ALL_NO_LIB) #Disable Boost Microsoft magic, all dependencies are handled by cmake

option(NO_BARE_METAL_TESTS "Disable bare metal test" OFF)
if (NO_BARE_METAL_TESTS)
    add_definitions(-DNO_BARE_METAL_TESTS)
endif()

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/../src/templates/licensecc_properties_test.h.in" 
	"${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp"
)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include/$<CONFIG>/licensecc_properties_test.h" 
		INPUT "${CMAKE_BINARY_DIR}/licensecc_properties_test.h.tmp")
include_directories ( ${CMAKE_BINARY_DIR}/include/$<CONFIG> )

add_subdirectory(library)
add_subdirectory(functional)