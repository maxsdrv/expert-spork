# @file
# @brief CPack settings decribed here
#
# @copyright (C) 2019 MPK Software

# The following directory structure is implies
# ${DEPLOY_SOURCES}
#   - Debian
#       - postinst
#       - postrm
#       - prerm
#   - Redhat
#       - postinst
#       - postrm
#       - prerm
#   - images
#       - logo.ico
#       - logo.png
#   - description.txt
#   - README

# Supported the following generators:
# NSIS - nsis installer for windows
# DEB - debian package (Debian-like distros)
# RPM - rpm package (Redhat-like distros)
# TGZ - portable tar gzip version
macro(definePlatformSpecificOptionsAndPackageVars)
    if("${PLATFORM}" MATCHES "Windows")
        set(OPTION_PACK_INCLUDE_TOPDIR ON)
        set(OPTION_PACK_GENERATOR "NSIS;ZIP")
        setupNsis()
    else()
        set(OPTION_PACK_INCLUDE_TOPDIR OFF)

        if ("${PLATFORM}" MATCHES "Debian|Ubuntu|Astra")
            set(OPTION_PACK_GENERATOR "DEB")
            setupDeb()
        elseif("${PLATFORM}" MATCHES "Altlinux|Redhat|MSVS")
            set(OPTION_PACK_GENERATOR "RPM")
            setupRpm()
        else()
            set(OPTION_PACK_GENERATOR "TGZ")
            setupTgz()
        endif()
    endif()
endmacro()

# Reset Cpack configuration to defaults values
macro(initCpackConfiguration)
    set(CPACK_IGNORE_FILES "")
    set(CPACK_INSTALLED_DIRECTORIES "")
    set(CPACK_SOURCE_IGNORE_FILES "")
    set(CPACK_SOURCE_INSTALLED_DIRECTORIES "")
    set(CPACK_STRIP_FILES "")
    set(CPACK_SOURCE_TOPLEVEL_TAG "")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "")
    set(CPACK_INSTALL_PREFIX "")
    set(CPACK_PACKAGING_INSTALL_PREFIX "")
    set(CPACK_SET_DESTDIR ON)
endmacro()

macro(definePackageVars PACKAGE_NAME DEPLOY_DATA_PATH)

    # Define general cpack variables
    set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
    set(CPACK_PACKAGE_VENDOR "MPK Software")
    set(CPACK_PACKAGE_CONTACT "MPK Software Company <support@mpksoft.ru>")

    set(CPACK_PACKAGE_VERSION "${VERSION}-${REVISION}")
    set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${REVISION}")

    set(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PACKAGE_NAME}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PACKAGE_NAME}")

    # Define cpack variables from common deploy source
    set(LICENSE_FILE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
    if(EXISTS ${LICENSE_FILE})
        set(CPACK_RESOURCE_FILE_LICENSE "${LICENSE_FILE}")
    endif()

    # Define cpack variables from specific deploy source
    if (EXISTS ${DESCRIPTION_FILE})
        file(STRINGS ${DESCRIPTION_FILE} PROJECT_DESCRIPTION NEWLINE_CONSUME)
        set(CPACK_PACKAGE_DESCRIPTION_FILE ${DESCRIPTION_FILE})
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
    endif()

    if (EXISTS ${README_FILE})
        set(CPACK_RESOURCE_FILE_README  "${README_FILE}")
        set(CPACK_RESOURCE_FILE_WELCOME "${README_FILE}")
    endif()

    if (EXISTS ${PACKAGE_ICON})
        get_filename_component(META_PACKAGE_ICON_FILE ${PACKAGE_ICON} NAME)
        set(CPACK_PACKAGE_ICON "${PACKAGE_ICON}")
    endif()
endmacro()

# This macro generates package script
macro(generatePackageScript DEPLOY_SOURCE VARNAME FILENAME)
    set(SCRIPT_SRC "${DEPLOY_SOURCE}/${FILENAME}.in")

    if(EXISTS ${SCRIPT_SRC})
        set(${VARNAME}_OUTPUT "${GENERATED_FILES}/${PACKAGE_NAME}/${FILENAME}")
        configure_file("${SCRIPT_SRC}" "${${VARNAME}_OUTPUT}" @ONLY)
    endif()
endmacro()

macro(generatePackageScripts DEPLOY_SOURCE)
    # Definitions used in DEB and RPM specific options
    generatePackageScript(${DEPLOY_SOURCE} META_PRE_INSTALL_SCRIPT_FILE    preinst)
    generatePackageScript(${DEPLOY_SOURCE} META_PRE_UNINSTALL_SCRIPT_FILE  prerm)
    generatePackageScript(${DEPLOY_SOURCE} META_POST_INSTALL_SCRIPT_FILE   postinst)
    generatePackageScript(${DEPLOY_SOURCE} META_POST_UNINSTALL_SCRIPT_FILE postrm)
    generatePackageScript(${DEPLOY_SOURCE} META_POST_UPGRADE_SCRIPT_FILE   postupgrade)
    generatePackageScript(${DEPLOY_SOURCE} META_PRE_UPGRADE_SCRIPT_FILE    preupgrade)
    generatePackageScript(${DEPLOY_SOURCE} META_PRE_UPGRADE_SCRIPT_FILE    conffiles)
endMacro()

# Check if CPack is installed and create pack target if no specified
macro(createPackTarget)
    # Check if pack target created for cmake
    if (NOT TARGET pack)
        add_custom_target(pack)
        set_target_properties(pack PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD true)
    endif()
endmacro()

# Add CPack target to make
macro(pack PACKAGE_NAME DEPLOY_DATA_PATH )
    createPackTarget()
    initCpackConfiguration()
    generatePackageScripts(${DEPLOY_DATA_PATH})
    definePackageVars(${PACKAGE_NAME} ${DEPLOY_DATA_PATH})

    # Define platform dependent options
    definePlatformSpecificOptionsAndPackageVars()

    # Execute CPack
    set(CUSTOM_CPACK_CONFIG_FILE "${PROJECT_BINARY_DIR}/CPackConfig_${PACKAGE_NAME}.cmake")
    set(CPACK_OUTPUT_CONFIG_FILE ${CUSTOM_CPACK_CONFIG_FILE})
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ${OPTION_PACK_INCLUDE_TOPDIR})
    set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ${OPTION_PACK_INCLUDE_TOPDIR})
    set(CPACK_GENERATOR "${OPTION_PACK_GENERATOR}")
    set(CPack_CMake_INCLUDED FALSE)
    include(CPack)

    set(PACK_TARGET_NAME "${PACKAGE_NAME}_pack")

    if(CMAKE_CROSSCOMPILING)
        set(CROSS_COMMAND_ENV ${CMAKE_COMMAND} -E env
            PATH="$ENV{PATH}"
            DPKG_ROOT="$ENV{DPKG_ROOT}"
            DPKG_ADMINDIR="$ENV{DPKG_ADMINDIR}"
            DEB_BUILD_ARCH="$ENV{DEB_BUILD_ARCH}"
            DEB_HOST_ARCH="$ENV{DEB_HOST_ARCH}"
            DEB_TARGET_GNU_TYPE="$ENV{DEB_TARGET_GNU_TYPE}")
    endif()

    # Package target
    add_custom_target(
        ${PACK_TARGET_NAME}
        COMMAND ${CROSS_COMMAND_ENV}
                ${CPACK_EXECUTABLE} --config ${CUSTOM_CPACK_CONFIG_FILE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
    set_target_properties(${PACK_TARGET_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD true)

    # Set dependencies
    add_dependencies(pack ${PACK_TARGET_NAME})
endmacro()

# Extract component dependencies
function(resolveComponentDependencies COMPONENTS DEPENDENCIES)
    set(_dependencies ${COMPONENTS})

    foreach(component ${COMPONENTS})
        if (TARGET ${component})
            get_property(component_deps TARGET ${component} PROPERTY DEPENDS_OF)
            if(component_deps)
                set(_dependencies "${_dependencies}" "${component_deps}")
            endif()
        endif()
    endforeach()

     list(REMOVE_DUPLICATES _dependencies)
     set(${DEPENDENCIES} ${_dependencies} PARENT_SCOPE)
endfunction()

# Create custom package, include only listed components
# PACKAGE_NAME - name of package
# GROUP_NAME - components group name
# Unparsed arguments is the list of components being included to package
function(createPack PACKAGE_NAME GROUP_NAME)
    include(CPackComponent)
    set(PACK_COMPONENT_INSTALL ON)
    message(STATUS "Packing ${PACKAGE_NAME}")

    resolveComponentDependencies("${ARGN}" _components)

    foreach(component ${_components})
        message(STATUS "  Adding ${component}")
        cpack_add_component(${component} GROUP ${GROUP_NAME})
    endforeach()

    set(CPACK_COMPONENTS_ALL ${_components})

    if (NOT DEPLOY_DATA_PATH)
        set(DEPLOY_DATA_PATH "${CMAKE_SOURCE_DIR}/deploy/${PLATFORM}/${PACKAGE_NAME}")
    endif()
    
    if(EXISTS "${DEPLOY_DATA_PATH}/config.cmake")
        include(${DEPLOY_DATA_PATH}/config.cmake)
    endif()
    pack(${PACKAGE_NAME} ${DEPLOY_DATA_PATH})
endfunction()
