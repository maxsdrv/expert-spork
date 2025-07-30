#! /bin/sh
# -*- coding: utf-8 -*-

set -x;

remote=origin
target_branch=${CI_MERGE_REQUEST_TARGET_BRANCH_NAME:-$(basename $(git symbolic-ref refs/remotes/$remote/HEAD))}
current_branch=${CI_MERGE_REQUEST_SOURCE_BRANCH_NAME:-$(git branch --show-current)}

# Delete Git LFS hooks
[ -e .git/hooks/post-checkout ] && rm .git/hooks/post-checkout

git fetch $remote || exit 1
git checkout $target_branch || exit 1
git pull || exit 1
git checkout $current_branch || exit 1

target_branch_head=$(git show-ref $target_branch --heads --hash)
merge_base_target_branch=$(git merge-base $target_branch $current_branch)

if [ "$target_branch_head" == "$merge_base_target_branch" ]; then
	exit 0;
fi

echo "Need run for current branch: git rebase $target_branch"
exit 1;
