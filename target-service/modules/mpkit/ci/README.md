# CI/CD

CI configs, scripts, templates... e.t.c. common for various projects

## Table of Contents

[TOC]

## Static checks

At the moment we support we following static code checks:
* `clang-format` to check proper code formatting
* `cmake-format` to check proper CMake files formatting
* `cspell` to check correct spelling
* `clang-tidy` to perform static code analysis
* `clazy` to perform Qt specific static code analysis

**Note**: to make your checks work properly one should  put corresponding config file to project root folder (see [options](#defining-static-checks-options))

### Exclude files from check

To exclude specific files from check just put its relative path to `.linterignore` file in project root folder

### Adding static checks to your project CI

To add static checks one should proceed with the following steps:

* add `mpk/ci` repo as submodule `ci` (concrete remote depends on your project path)

Note: always use relative path to add submodule, not absolute 

```sh
git submodule add ../../mpk/ci ci 
```
or

```sh
git submodule add ../../../mpk/ci ci 
```

depending on your project groups nesting structure

* add to your `.gitlab-ci.yml`

```yml
variables:
  ...
  CI_PATH: ./ci/
  ...
stages:
  ...
  - analysis
  ...
include:
  ...
  - project: 'mpk/ci'
    file: '/static-checks.yml'
  - project: 'mpk/ci'
    file: '/static-analysis.yml'
  ...
```

`static-checks.yml` will enable `clang-format`, `cspell` and `cmake-format` jobs, while `static-analysis.yml` will enable `clang-tidy` and `clazy` in single job

`static-checks` performs on `${CI_REGISTRY}/mpk/ci/static-check-image:latest` and usually you do not have to care about its parameters

`static-analysis` in turn may require additional setup because it performs cmake step, so some projects may require additional libs or cmake parameters. 

By default `static-analysis` uses `${CI_REGISTRY}/mpk/internal/arch-builder:latest`. To use another docker image define the following variable in you `.gitlab-ci.yml`:

```yml
variables:
  ...
  STATIC_ANALYSIS_IMAGE: ${YOUR_BUILDER}
  ...
```

By default `static-analysis` call cmake with the following args:

```
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_BUILD_TYPE=Debug \
-DQT_QMAKE_EXECUTABLE=qmake \
-DBUILD_EXAMPLES=ON \
-DBUILD_TESTS=ON 
```

To use another params define the following variable in you `.gitlab-ci.yml`:

```yml
variables:
  ...
  CMAKE_ARGS: "<your params goes here>"
  ....
```

Other variables:

* `CHECK_FOLDERS:"src"` - folders exposed to linter
* `CHANGES_ONLY:true` - check only changed files
* `TARGET_BRANCH_NAME` - origin's branch name to match against (if CHANGES_ONLY is set)
* `CHECK_SUBMODULES:false` - check also submodules
* `JOBS_NUMBER:$(nproc)` - parallel jobs count
* `CHANGED_FILES_FILTER:"*.h *.hpp *.cpp"` - check only files in filter
* `LINTER_DEBUG:false` - print debug info
* `CMAKE_FORMAT_AUTO_FIX:true` - apply formatting to local files (for `cmake-format` only) 
* `AUTO_FORMAT:false` - apply formatting to local files (for `clang-format` only) 
  
### Run only changed files

According [https://docs.gitlab.com/ee/ci/yaml/includes.html#override-included-configuration-values](https://docs.gitlab.com/ee/ci/yaml/includes.html#override-included-configuration-values)
you can add your settings to current jobs:

```yml
code-lint/mr:
  only:
    refs:
      - merge_requests
    changes:
      - src/*
```

In this example job `code-lint/mr` run in merge_requests and if any files was changed in `src/*`

### Running static checks locally

To run static checks locally one may use `scripts/docker_run_<tool>.sh` Script should be run from project root folder:

```sh
./${CI_PATH}/scripts/docker_run_clazy.sh
```

They should be run on the same docker images as CI to ensure that analysis results are not diverged.

**Note**: corresponding docker images should be pulled manually

```sh
docker login registry.dev.mpksoft.ru
docker pull registry.dev.mpksoft.ru/mpk/ci/static-check-image:latest
``` 

To run static analysis with custom image and/or custom cmake command one should pass these parameters as environment variables:

```sh
env STATIC_ANALYSIS_IMAGE=<my image> env CMAKE_ARGS=<my args> ./${CI_PATH}/scripts/docker_run_clang_tidy.sh
```

### Adding to Qt creator as a build step

* go to `Projects->Build->Build Steps`
* `Add Build Step` -> `Custom Process Step`
* Use the following parameters:
  * Command: `/bin/bash`
  * Arguments: `-c "env STATIC_ANALYSIS_IMAGE=\"registry.dev.mpksoft.ru/<your_build_image>\" %{sourceDir}/ci/scripts/docker_run_clang_tidy.sh"`
  * Working directory: `%{sourceDir}`

Use the same steps to add additional jobs. Any additional options (`CHECK_SUBMODULES`, `AUTO_FORMAT` etc) can be set via env var in arguments:
`-c "env CHECK_SUBMODULES=true env AUTO_FORMAT=true %{sourceDir}/ci/scripts/docker_run_clang_format.sh"`

### Defining static checks options

* `clang-format` options are defined in `.clang-format` file in project root folder
* `cmake-format` options are defined in `.cmake-format.py` file in project root folder
* `cspell` options are defined in `.cspell.json` file in project root folder
* `clang-tidy` options are defined in `.clang-tidy` file in project root folder
* `clazy` options are defined in `.clazy` file in project root folder

Examples can be found [here](configs)

To exclude any file from check just put the file `.linterignore` to the project root folder, list files with relative paths there (i.e. `include/mssc-link/mssc-link.h`)

## Image updates

To update image to recent tools versions just run corresponding job in manual mode. 

To put specific tag to docker image define `IMAGE_TAG` variable while running the job.

## Check rebase in MR

* add to your `.gitlab-ci.yml`

```yml
variables:
  ...
  CI_PATH: ./ci/
  ...
stages:
  - repository-check
  ...
include:
  ...
  - project: 'mpk/ci'
    file: '/check-rebase.yml'
  ...
```

If your merge request requires rebase you will get error like this: 

```
Need run for current branch: git rebase develop
```

To run checks locally you can use `scripts/check_rebase.sh`. Script should be run from project root folder:

```sh
./${CI_PATH}/scripts/check_rebase.sh
```
