# @file
# @brief Include licensecc library and binaries to project
# https://open-license-manager.github.io/
#
set(LICENSECC_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/licensecc)
set(LICENSECC_PROJECTS_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/license)

set(OPENSSL_USE_STATIC_LIBS ON)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

if (UNIX)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework system filesystem)
endif()

include(ExternalProject)

ExternalProject_Add(licensecc
    SOURCE_DIR "${LICENSECC_SOURCE}"
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${LICENSECC_INSTALL}"
               "-DLCC_PROJECT_NAME=${LCC_PROJECT_NAME}"
               "-DLCC_PROJECTS_BASE_DIR=${LICENSECC_PROJECTS_BASE_DIR}"
               "-DBUILD_TESTING=OFF"
    CMAKE_CACHE_ARGS "-DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}"
               "-DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}"
               "-DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}"
)

message("Licensecc install directory: ${LICENSECC_INSTALL}")
include_directories(${LICENSECC_INSTALL}/include)
include_directories(${LICENSECC_INSTALL}/include/licensecc/${LCC_PROJECT_NAME})

# Import libraries from External Project required by Target Project
add_library(licensecc_static STATIC IMPORTED)
if (UNIX)
    set(LIBRARY_DIR lib)
endif()

if (WIN32)
    set(EXTERNAL_LIBS "-lbcrypt;-lcrypt32;-lws2_32;-liphlpapi;-lz")
endif()

set_target_properties(licensecc_static PROPERTIES
    IMPORTED_LOCATION ${LICENSECC_INSTALL}/${LIBRARY_DIR}/licensecc/${LCC_PROJECT_NAME}/liblicensecc_static.a
    INTERFACE_LINK_LIBRARIES "OpenSSL::Crypto;-lpthread;dl;${EXTERNAL_LIBS}")

macro(installLccinspector BINARY_PATH)
    if (UNIX)
        install(PROGRAMS "${LICENSECC_INSTALL}/bin/lccinspector"
                DESTINATION "${BINARY_PATH}"
                COMPONENT "lccinspector")
    endif()
endmacro()
