#! /usr/bin/env bash
# -*- coding: utf-8 -*-

# Debug routines
debug=${LINTER_DEBUG:-false}
# print debug output to stderr
# $1 - log string
log_debug() {
    if [ "${debug}" == "true" ]; then
        echo "$1" >&2
    fi
}

# Git routines
# fetch latest changes for target branch from remote
# $1 - target branch to fetch
remote=origin
git_fetch_latest() {
    if [ "${LOCAL}" != "true" ]; then
        if [ "${debug}" == "true" ]; then
            echo "Fetching ${remote}/$1" >&2
        fi
        git fetch ${remote} $1
    fi
}

# print origin's HEAD branch name
git_head_branch_name() {
    basename $(git symbolic-ref refs/remotes/${remote}/HEAD)
}

# print changed file's names against remote target branch
# $1 - target branch to match against
git_list_changes() {
    git diff --no-commit-id --name-only --diff-filter=d -r ${remote}/$1 --
}

# print project submodules list
git_list_submodules() {
    git ls-files --stage | grep ^160000 | awk '{ print $4 }'
}


# list files for single repo
# $1 - check_folders list
# $2 - file pattern
# $3 - check changes only flag
# $4 - target branch name (only useful if checking changes only)
list_files_for_repo() {
    local check_folders=$1
    local file_patterns=$2
    local changes_only=$3
    local target_branch=$4

    log_debug "list_files_for_repo $*"

    # Fetch source list from git changes, if 'changes only' flag is set, or by
    # listing all files in supplied folders
    if [ "${changes_only}" == "true" ]; then
        git_fetch_latest ${target_branch}
        local folders_pattern=$(echo "^${check_folders}" | sed -e "s!,!|^!")
        log_debug "Folders pattern: ${folders_pattern}"
        local source_list=$(git_list_changes ${target_branch} | grep -E ${folders_pattern})
    else
        local paths=$(echo "${check_folders}" | sed -e "s!,! !")
        log_debug "Paths: ${paths}"
        local source_list=$(find ${paths} -type f)
    fi

    if [ -z "${source_list}" ]; then
        log_debug "No source files"
        return
    fi

    log_debug "Source files:"
    log_debug "${source_list}"

    # Match only files that are supposed to be consumed by linter,
    # typically that are C/C++ sources and headers
    local matched_list=$(echo "${source_list}" | grep -E ${file_patterns})

    if [ -z "${matched_list}" ]; then
        log_debug "No matched files"
        return
    fi

    log_debug "Matched files:"
    log_debug "${matched_list}"

    # Read list of files to ignore
    if [ -f ${ignore_file} ]; then
        local ignored_list=$(cat ${ignore_file})
    elif [ -f ${ignore_file_old} ]; then
        echo "Using ${ignore_file_old} is deprecated, use ${ignore_file} instead" >&2
        local ignored_list=$(cat ${ignore_file_old})
    fi

    # Filter out ignored files
    if [ "${ignored_list}" ]; then
        log_debug "Ignored files:"
        log_debug "${ignored_list}"
        comm -23 <(echo "${matched_list}" | sort) <(echo "${ignored_list}" | sort)
    else
        log_debug "No ignored files"
        echo "${matched_list}"
    fi

}

# list files for project
# $1 - check folders list
# $2 - file pattern
# $3 - check changes only flag
# $4 - target branch name (only useful if checking changes only)
# $5 - check submodules flag
list_files() {
    local check_folders=$1
    local file_patterns=$2
    local changes_only=$3
    local target_branch=$4
    local check_submodules=$5

    if [ "${debug}" == "true" ]; then
        echo "list_files $*" >&2
    fi

    # List main project's files
    list_files_for_repo ${check_folders} ${file_patterns} ${changes_only} ${target_branch}

    # List submodule's files
    if [ "${check_submodules}" == "true" ]; then
        log_debug "Checking submodules:"
        for submodule in $(git_list_submodules)
        do
            pushd ${submodule} >/dev/null 2>&1
            log_debug "[${submodule}]"
            # Submodules can only be checked for default folders and against head branch name
            list_files_for_repo ${check_folders_default} ${file_patterns} ${changes_only} $(git_head_branch_name) | sed -e "s!^!${submodule}/!"
            popd >/dev/null 2>&1
        done
    fi
}

# Project's folders supplied to linter checks
# Format: "path1,path2,..."
# Example: "src,tests,examples"
check_folders_default="src"
check_folders=${CHECK_FOLDERS:-${check_folders_default}}

# Collected files patterns
# Format: "\PATTERN1$|\PATTERN2$..." (grep format)
# Example: "\.h$|\.hpp$|\.cpp$"
file_patterns_default="\.h$|\.hpp$|\.cpp$"
file_patterns=${CHANGED_FILES_FILTER:-${file_patterns_default}}

# Changes only flag
# If set, only files that was changed relative to the origin are collected
changes_only=${CHANGES_ONLY:-true}

# Target branch name
# If 'changes only' is set, specifies origin's branch name to match against
# If not set explicitly, then branch name from CI env is used, if latter not set
# either, then origin's head branch name is used instead
target_branch_default=${CI_MERGE_REQUEST_TARGET_BRANCH_NAME:-$(git_head_branch_name)}
target_branch=${TARGET_BRANCH_NAME:-${target_branch_default}}

# Check submodules flag
# If set, recursively collect files from every registered submodule
# Submodule's folders are always default ones and submodule's branch name is
# always origin's head
check_submodules=${CHECK_SUBMODULES:-false}

# List of project files excluded from linter checks
ignore_file=".linterignore"
ignore_file_old="exclude_from_checks.lst"

# Jobs count
jobs_count=${JOBS_NUMBER:-$(nproc)}

files=$(list_files ${check_folders} ${file_patterns} ${changes_only} ${target_branch} ${check_submodules})
log_debug "Files to process:"
log_debug "${files}"

echo ${files} | xargs --no-run-if-empty -n 1 -P ${jobs_count} --verbose $@
