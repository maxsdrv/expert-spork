# @file
# @brief nsis specific CPack configuration
#
# NSIS package
#

macro(setupNsis)
    # Fix icon path
    if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows" AND CPACK_PACKAGE_ICON)
        # NOTE: for using MUI (UN)WELCOME images we suggest to replace nsis defaults,
        # since there is currently no way to do so without manipulating the installer template (which we won't).
        # http://public.kitware.com/pipermail/cmake-developers/2013-January/006243.html

        # SO the following only works for the installer icon, not for the welcome image.

        # NSIS requires "\\" - escaped backslash to work properly. We probably won't rely on this feature,
        # so just replacing / with \\ manually.

        #file(TO_NATIVE_PATH "${CPACK_PACKAGE_ICON}" CPACK_PACKAGE_ICON)
        string(REGEX REPLACE "/" "\\\\\\\\" CPACK_PACKAGE_ICON "${CPACK_PACKAGE_ICON}")
    endif()

    # Fix installation path for x64 builds
    if(X64)
        # http://public.kitware.com/Bug/view.php?id=9094
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    endif()

    # Package options
    set(CPACK_NSIS_DISPLAY_NAME  "${DISPLAY_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME  "${PACKAGE_NAME}")
    set(CPACK_NSIS_MUI_ICON      "${PACKAGE_ICON}")
    set(CPACK_NSIS_MUI_UNIICON   "${PACKAGE_ICON}")
endmacro()
