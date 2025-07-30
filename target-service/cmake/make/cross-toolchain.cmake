#
# CMake Toolchain file for crosscompiling
#
# Add Cross-Compilation support when the environment variables
# TARGET_HOST and SYSROOT are defined

#
# Check required environment variables
#
if("$ENV{SYSROOT}" STREQUAL "")
    message(FATAL_ERROR "Environment variable SYSROOT is not defined.")
    return()
endif()
# Sysroot
set(CMAKE_SYSROOT "$ENV{SYSROOT}")

# Host GNU type
if("$ENV{TARGET_HOST}" STREQUAL "")
    message(FATAL_ERROR "Environment variable TARGET_HOST is not defined.")
else()
    # Compilers name
    if (NOT DEFINED CMAKE_C_COMPILER)
        set(CMAKE_C_COMPILER $ENV{TARGET_HOST}-gcc)
    endif()
    message("CMAKE_C_COMPILER is set to ${CMAKE_C_COMPILER}")

    if (NOT DEFINED CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER $ENV{TARGET_HOST}-g++)
    endif()
    message("CMAKE_CXX_COMPILER is set to ${CMAKE_CXX_COMPILER}")

    set(CMAKE_ASM_COMPILER_TARGET $ENV{TARGET_HOST})

    set(CMAKE_C_COMPILER_TARGET $ENV{TARGET_HOST})
    set(CMAKE_CXX_COMPILER_TARGET $ENV{TARGET_HOST})
endif()

# Retrieve the machine supported by the toolchain
set(TOOLCHAIN_MACHINE $ENV{TARGET_HOST})
message("-- Target machine: ${TOOLCHAIN_MACHINE}")

string(REGEX REPLACE "-.*" "" TOOLCHAIN_ARCH "${TOOLCHAIN_MACHINE}")
message("-- Target achitecture: ${TOOLCHAIN_ARCH}")

# Target operating system name.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_PROCESSOR ${TOOLCHAIN_ARCH})

if (CMAKE_C_COMPILER MATCHES "clang")
    set(ENV{CFLAGS} "--target=$ENV{TARGET_HOST} $ENV{CFLAGS}")
    message("CFLAGS is set to $ENV{CFLAGS}")
endif()

if (CMAKE_CXX_COMPILER MATCHES "clang")
    set(ENV{CXXFLAGS} "--target=$ENV{TARGET_HOST} $ENV{CXXFLAGS}")
    message("CXXFLAGS is set to $ENV{CXXFLAGS}")
endif()

# Where to look for the target environment. (More paths can be added here)
set(CMAKE_FIND_ROOT_PATH "/usr/${TOOLCHAIN_MACHINE};${CMAKE_SYSROOT}"
    CACHE PATH "List of root paths to search on the filesystem")

set(CMAKE_INCLUDE_PATH  /usr/include/${TOOLCHAIN_MACHINE})
set(CMAKE_LIBRARY_PATH  /usr/lib/${TOOLCHAIN_MACHINE})
set(CMAKE_PROGRAM_PATH  "/usr/bin/${TOOLCHAIN_MACHINE};/usr/bin")

#
# Configure pkg-config for cross-compilation
#
if(IS_DIRECTORY "${CMAKE_SYSROOT}/usr/lib/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} ${CMAKE_SYSROOT}/usr/lib/pkgconfig)
endif()
if(IS_DIRECTORY "${CMAKE_SYSROOT}/usr/lib/${TOOLCHAIN_MACHINE}/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} ${CMAKE_SYSROOT}/usr/lib/${TOOLCHAIN_MACHINE}/pkgconfig)
endif()
if (NOT "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
    # Don't strip -I/usr/include out of cflags
    set(ENV{PKG_CONFIG_ALLOW_SYSTEM_CFLAGS} 0)
    # Don't strip -L/usr/lib out of libs
    set(ENV{PKG_CONFIG_ALLOW_SYSTEM_LIBS} 0)
endif()

# Workaround as some pkgconfig file forgot to add the architecture specific include 
# folder such as openssl.pc
include_directories(${CMAKE_SYSROOT}/usr/include ${CMAKE_SYSROOT}/usr/include/${TOOLCHAIN_MACHINE})

# Workaround as some library are installed in ${CMAKE_SYSROOT}/lib/${TOOLCHAIN_MACHINE}
# such as libpcre.so (required by glib-2.0)
link_directories(${CMAKE_SYSROOT}/lib/${TOOLCHAIN_MACHINE})
link_directories(${CMAKE_SYSROOT}/usr/lib/${TOOLCHAIN_MACHINE})

# Adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries in the target environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#
# Set architecture for CPack
#
if ("${TOOLCHAIN_MACHINE}" STREQUAL "arm-linux-gnueabihf")
    set(CPACK_PACKAGE_ARCHITECTURE        armhf)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE    armv7hl)
    set(CPACK_SYSTEM_NAME "linux-armhf")
elseif ("${TOOLCHAIN_MACHINE}" STREQUAL "aarch64-linux-gnu")
    set(CPACK_PACKAGE_ARCHITECTURE        arm64)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE    aarch64)
    set(CPACK_SYSTEM_NAME "linux-arm64")
endif()

#
# Prepare custom DPKG tools
#
set(ENV{PATH} "${CMAKE_SOURCE_DIR}/cmake/tools:$ENV{PATH}")
set(ENV{DPKG_ROOT} "${CMAKE_SYSROOT}")
set(ENV{DPKG_ADMINDIR} "/var/lib/dpkg")

set(ENV{DEB_BUILD_ARCH} $ENV{TARGET_HOST})
set(ENV{DEB_HOST_ARCH} $ENV{TARGET_HOST})
set(ENV{DEB_TARGET_GNU_TYPE} $ENV{TARGET_HOST})

#
# Add gtest options
#
set(GTEST_OPTIONS "-DCMAKE_C_FLAGS:STRING=$ENV{CFLAGS} "
                  "-DCMAKE_CXX_FLAGS:STRING=$ENV{CXXFLAGS}")

#
# Set emulator environment
#
set(CMAKE_CROSSCOMPILING_EMULATOR /usr/bin/qemu-aarch64 -L ${CMAKE_SYSROOT})
