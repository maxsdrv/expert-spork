# gqtest
Extending gtest with Qt support: comparators, printers, event loop

### Requirements
* cmake ver. >= 3.9
* c++ compiler with c++14 support. Tested on gcc 7.0+ and clang 7.0+
* Qt ver. >= 5.7 (optional)

### Build

* clone project
* clone [Google Test Project](https://dev.mpksoft.ru/mpk/googletest) to `<gmock_source>`
* build with GMOCK_SRC set to `<gmock_source>`
* `cmake -B<build_dir> -DGMOCK_SRC=<gmock_source> -DQT_QMAKE_EXECUTABLE=qmake`
* `cmake --build <build_dir>`
* test build: `pushd <build_dir> && ctest --output-on-failure`


### Usage
* clone project to `<gqtest_source>`
* clone [Google Test Project](https://dev.mpksoft.ru/mpk/googletest) to `<gmock_source>`
* set GMOCK_SRC set to `<gmock_source>`
* include `<gqtest_source>/gqtest.cmake`
* 3 macros are added:
 * * `tests(<SUBDIR>, [BUILD_DIR])` - add subdirectory SUBDIR with tests if BUILD_TESTS is ON. If SUBDIR is not direct subdirectory of CMAKE_CURRENT_SOURCE_DIR one should define BUILD_DIR
 * * `gtest(<TARGET> [SOURCES <source_list>] [GLOBBING <globbing_pattern>] [LIBS <libs_list>] [GMOCK <TRUE|FALSE>])` - add single executable <TARGET> with google tests. 
 * * * `source_list` - list of source files to add to TARGET
 * * * `globbing_pattern` - pattern for searching and adding sources to TARGET
 * * * `libs_list` - list of libraries to link to TARGET
 * * * `GMOCK`: if set to TRUE, GMOCK_LIBRARY is linking and GMOCK_MAIN is using instead of GTEST_MAIN
 * * `qtest(<TARGET> [SOURCES <source_list>] [GLOBBING <globbiing_pattern>] [LIBS <libs_list>] [GMOCK <TRUE|FALSE>] [QT <qtmodules_list>])` - same as gtest but add Qt support, link gqtest library to TARGET and allow specify witch Qt modules should be linked to TARGET
