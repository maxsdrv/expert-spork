#!/bin/bash

# @file
# @brief Script to publish new version to master branch
#
# @copyright (C) 2019 MPK Software


if [ "$#" -ne 2 ]
  then
    echo "Merge develop into master and publish new versions. Invalid number of arguments.\n Usage: publish.sh <stable version> <develop version>"
  exit  
fi

pushd $(dirname $0)/..

#store current branch
current=$(git rev-parse --abbrev-ref HEAD)

#switch to new-develop
git checkout -b develop_for_merge origin/develop
git checkout master
git pull

git merge develop_for_merge
git push origin master

oldversion=$1
newversion=$2

git tag -a $oldversion -m "Version $oldversion Release"
git push origin $oldversion

git tag -a $newversion -m "Version $newversion Develop"
git push origin $newversion

git checkout $current

git branch -D develop_for_merge

popd
