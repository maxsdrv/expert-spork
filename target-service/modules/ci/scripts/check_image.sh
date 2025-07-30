#! /usr/bin/env bash
# -*- coding: utf-8 -*-

image=$1

if ! docker image inspect $image > /dev/null; then
    echo "Pull image with: 'docker pull $image command'"
    exit 1
fi
