# DSS target provider

## Development
### Profiles
`devspace dev` and `devspace deploy` commands (*described next*) can be
run with various configurations - profiles by adding -p option as follows:
```bash
devspace -p <profile name> dev
```
Available profiles:
- `tamerlan` - Tamerlan-L sensor
- `radar-iq` - Radar IQ sensor

By default, "rest imitator" is used.

### Local build and run
Considering you have run imitators and deployed all services in the
`local-env` directory you can replace the target-provider service with
locally built development instance by running
```bash
devspace purge
devspace dev
```
By default, `build-dev` directory is used for CMake build directory.
You can change it by `export BUILD_DIR=my-build-dir`

To change logging level
```bash
devspace dev LOG_LEVEL=TRACE
```

To change target host address
```bash
devspace dev TARGET_HOST=1.2.3.4
```

#### Deploy locally build instance
```bash
devspace build
devspace deploy
```

### Testing
#### API Tests
[Readme](tests/api/README.md)

## Tools
### Code Generation
#### Prerequisites
> * `rct-apps` git repo must be located at `../../rct/rct-apps` relative to this repo

To regenerate code for dependency APIs run
```bash
devspace run gen
```
### Sources linting
You can run linters clang-tidy, clazy and also linters for
OpenAPI & AsyncAPI specs by
```bash
devspace run lint [file]
```

### Sources formatting
```bash
devspace run format [file]
```


## Annotation

// TODO: add project description here.

## Project structure

* dss-target-provider - provides unified REST API to manage detectors and jammers,
* dss-frontend - provides web-interface,
* dss-control - provdes an API to manage DSS Server environment,
* dss-access - authentication service.

## Project dependencies

// TODO: actualize dependencies

* Cmake >= 3.17
* Qt >= 5.15
* Clang >= 14.0 or GCC >= 12.2 (C++17 is mandatory)
* libboost >= 1.74

For complete list of build dependencies see Docker file.

## Build

### Obtain the sources

The commands to obtain the project source codes are given below:

```
git clone ssh://git@dev.mpksoft.ru:2222/tsnk/tts/dss/dss-target-provider.git && \
cd dss-target-provider && \
git submodule update --init --recursive
```

### Quick start workflow

The short command list to build the project is given below:

```
# Get the sources
git clone ssh://git@dev.mpksoft.ru:2222/tsnk/tts/dss/dss-target-provider.git && \
cd dss-target-provider && \
git submodule update --init --recursive

# Prepare build directory outside source fir
mkdir -p ../dss-target-provider-build

# Initialize build system (Debug mode)
cmake -B../dss-target-provider-build \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_QMAKE_EXECUTABLE=qmake \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON

# Build project
cmake --build ../dss-target-provider-build -- -j$(nproc) install

# Launch tests 
cmake --build ../dss-target-provider-build -- test

# Build packages
cmake --build ../dss-target-provider-build -- pack
```

### CMake keys

The following cmake key sets are provided for configuring the steps to build 
and install the application.

#### Build mode keys

* CMAKE_BUILD_TYPE - setup build mode: Release or Debug (Release by default);
* BUILD_EXAMPLES - defines whether or not to build project examples (possible 
values: ON, OFF);
* BUILD_TESTS - defines whether or not to build tests (possible values: ON, OFF);
* SANITIZE - build with address sanitizer (asan) (possible values: ON, OFF);

#### Pathes definition keys

* INSTALL_PREFIX - base path for BINARIES and LIBRARIES, e.g.: `/usr/local`;
* BINARY_PATH - path to executable, e.g.: `/usr/local/bin`;
* LIBRARY_PATH - path to shared libraries, e.g.: `/usr/local/lib`;
* CONFIG_PATH - path to not changable config files, e.g.: 
`${INSTALL_PREFIX}/etc/${PACKAGE_NAME}`;
* CACHE_PATH - path to changable configs, logs etc, e.g.: 
`${INSTALL_PREFIX}/var/lib/${PACKAGE_NAME}`;
* SHARED_PREFIX - base path for plugins, help and translations, e.g.: 
`${INSTALL_PREFIX}/share`;
* SHARED_PATH - path to static application's data, like examples, docs or 
licenses, e.g.: `${SHARED_PREFIX}/${PACKAGE_NAME}`;
* PLUGIN_PATH - path to application plugins, e.g.: 
`${INSTALL_PREFIX}/lib/$ {PACKAGE_NAME}/plugins`;
* TRANSLATION_PATH - path to translation files, e.g.: 
`${INSTALL_PREFIX}/share/$ {PACKAGE_NAME}/translations`;
* HELP_PATH - path to help files, e.g.:
`${INSTALL_PREFIX}/share/${PACKAGE_NAME}/doc`;
* LOG_PATH - path to log files, e.g.: 
`${INSTALL_PREFIX}/var/log/${PACKAGE_NAME}`;
* RUN_PATH - path to run-time variable files, e.g.: 
`${INSTALL_PREFIX}/var/run/${PACKAGE_NAME}`;
* TMP_PATH - path to temporary stuff, e.g.: `${INSTALL_PREFIX}/tmp`;

## Build dependencies

###  Ubuntu 22.04

The commands to install project dependencies for KUbuntu 22.04 are listed 
below:

```
apt-get update \
    && apt-get install -y \
        libboost-exception-dev \
        libboost-locale-dev \
        libboost-program-options-dev \
        libqt5websockets5-dev \
        libqt5serialbus5-dev \
        libqt5serialport5-dev \
        qtpositioning5-dev \
        libyaml-cpp-dev \
        libmodbus-dev
```

If any cmake errors occurred during installation it may be useful to to search 
a module by command:

```
apt-file search <file_name_wich_need_cmake>
```

Complete list of build dependencies may be found [here](./docker/intel/dss-ubuntu-builder/Dockerfile)

## Build project

Please never build the project inside the source directory, you must prepare 
the project build directory separately from its sources:

```
mkdir -p ../dss-target-provider-build
```

### Debug mode

The following commands are for setting the project into debug mode:

```
cmake -B../dss-target-provider-build \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQT_QMAKE_EXECUTABLE=qmake \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON
```

### Release mode

The typical commands for setting project into release mode are listed below:

```
cmake  -B../dss-target-provider-build \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_QMAKE_EXECUTABLE=qmake \
    -DINSTALL_PREFIX=/usr \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON
```

### Build and install

Always use `cmake --build ...` command for building, example is listed below:

```
cmake --build ../dss-target-provider-build -- -j$(nproc) install
```

The installation stage is relevant in debug mode because programms try to find
their configuration files in the installation path. Default installation 
prefixes:
  * for debug mode `~/.dss-target-provider`
  * for release mode `/usr/local`.
So `install` option in required.

### Build packages

Against to software installation via the build system that relevant for debug 
mode, the package building is more preferable into the release mode.

```
cmake --build ../dss-target-provider-build -- pack
```

After the command finished you can find the packages in the root of the build 
directory.

### Check installation

To check installation you can try to check `dss-target-service` version, the 
command below is actual for debug mode:

```
~/.dss-target-provider/bin/dss-target-service --version
```

## Project deployment

### Debugging environment

The debug environment is represented by the following services, which are 
deployed via docker-compose:

- dss-target-service - targer provider serice accessible by pors 19080, 19081;
- dss-frontend - web frontend service accessible on `http://localhost:9090`.

NOTE: one should be logged in to docker registry
```
docker login registry.dev.mpksoft.ru
```

To deploy debug environment one should launch `./init_environment.sh -m dev` 
script in the root of the sources directory.
* If you want to erase existing database add `-r hard` key: 
`./init_environment.sh -m dev -r hard`.
* If you want to rebuild docker images without erase existing database add 
`-r soft` key: `./init_environment.sh -m dev -r soft`.
* In order to run `frontend` or `imitators` serivces, add `-p` option to 
command line, like this: `./init_environment.sh -m dev -p frontend,imitators`.

## Extended information
