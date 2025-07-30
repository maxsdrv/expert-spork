#!/bin/bash

configs_path=$(dirname $0)
mkdir -p $(dirname $1)

echo "modules:" > $1

env_postfix="_DEVICES"
env | grep "$env_postfix" | sed "s/\(.*\)$env_postfix=.*/\1/" | \
while read -r config; do
  config_file=$(echo $config | tr '[:upper:]' '[:lower:]' | sed "s/_/-/")
  hosts=${config}_DEVICES
  echo "$config config enabled for hosts: ${!hosts}"

  if [ ! -x "$configs_path/render_config_$config_file.sh" ]; then
    echo Unknown device: $config
    continue
  fi
  echo "- adding config sections for $config_file devices"
  $configs_path/render_config_$config_file.sh ${!hosts} >> $1
done
true
