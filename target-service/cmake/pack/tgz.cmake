# @file
# @brief tgz specific CPack configuration
#
# Archives (zip, tgz, ...)
#

macro(setupTgz)
    set(CPACK_ARCHIVE_COMPONENT_INSTALL ${PACK_COMPONENT_INSTALL})
    set(CPACK_ARCHIVE_PACKAGE_RELOCATABLE ON)
endmacro()
