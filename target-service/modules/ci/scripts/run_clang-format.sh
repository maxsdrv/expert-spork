#! /usr/bin/env bash
# -*- coding: utf-8 -*-

script_dir=$(dirname -- "$0")

auto_format_option=false
if [ ! -z ${AUTO_FORMAT+x} ]; then
  auto_format_option=${AUTO_FORMAT,,}
fi


if [ "${auto_format_option}" = "true" ]; then
  run_mode='--i'
elif [ "${auto_format_option}" = "false" ]; then
  run_mode='--dry-run'
else
  echo "Invalid AUTO_FORMAT option. Expected true|false, got ${AUTO_FORMAT}"
  exit 1;
fi

clang-format --version && \
$script_dir/run_script.sh clang-format \
    ${run_mode} \
    --verbose \
    --Werror
