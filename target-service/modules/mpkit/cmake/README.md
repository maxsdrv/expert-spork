# cmake

This project contains a set of the cmake scripts for building C++/Go projects.

### Options

One can enable or disable cmake option by passing `-D<option>=ON|OFF (e.g. -DSANITIZE=ON)`

* BUILD_EXAMPLES - whether we should build examples if any (default=OFF)
* BUILD_TESTS - whether we should build tests if any (default=OFF)
* COVERAGE - whether we should build with gcov (default=OFF)
* INTERACTIVE_TRANSLATION - whether we should launch linguist for any untranslated ts/po file in cmake stage (default=ON)
* SANITIZE - whether we should build with Address Sanitizer support (clang only, default=OFF)
* TRANSLATE_RECURSIVE - whether we should run translate stage for all submodules as well (default=OFF)
* `_use_<item>` - if we add submodule project via `optional_item` macro, our build system automatically adds option `_use_<item>` controls whether we should build this item or not (usable for mpkit, default=OFF)
NOTE: we have two options to control behaviour of item
  * `<item>_install_headers`. If set to TRUE special target `<item>-dev` that copies header file it `INSTALL_PATH` will be added
  * `<item>_build_type` controls the type of library, either `STATIC` or `SHARED`
* VERBOSE_CMAKE - more verbose processing in cmake step

### Settings

Number of overridable settings

* INSTALL_PREFIX   - base path for BINARIES and LIBRARIES (`/usr/local`)
* BINARY_PATH      - path to executable (`${INSTALL_PREFIX}/bin`)
* LIBRARY_PATH     - path to shared libraries (`${INSTALL_PREFIX}/lib`)
* CONFIG_PATH      - path to not changable config files, e.g.: (`/etc/${PACKAGE_NAME}`)
* CACHE_PATH       - path to changable configs, logs etc, e.g. (`${INSTALL_PREFIX}/var/lib/${PACKAGE_NAME}`)
* SHARED_PREFIX    - base path for plugins, help and translations, e.g. (`${INSTALL_PREFIX}/share`)
* SHARED_PATH      - path to static application's data, like examples, docs or licenses, e.g.: (`${SHARED_PREFIX}/${PACKAGE_NAME}`)
* PLUGIN_PATH      - path to application plugins, e.g.: (`${LIBRARY_PATH}/${PACKAGE_NAME}/plugins`)
* TRANSLATION_PATH - path to translation files, e.g.: (`${INSTALL_PREFIX}/share/${PACKAGE_NAME}/translations`)
* HELP_PATH        - path to help files, e.g.: (`${INSTALL_PREFIX}/share/${PACKAGE_NAME}/doc`)
* LOG_PATH         - path to log files, e.g.: (`${INSTALL_PREFIX}/var/log/${PACKAGE_NAME}`)
* RUN_PATH         - path to run-time variable files, e.g.: (`${INSTALL_PREFIX}/var/run/${PACKAGE_NAME}`)
* TMP_PATH         - path to temporary stuff, e.g.: (`${INSTALL_PREFIX}/tmp`)
* INCLUDE_PATH     - path to includes directory (if we install dev package), e.g.: (`${INSTALL_PREFIX}/include`)
