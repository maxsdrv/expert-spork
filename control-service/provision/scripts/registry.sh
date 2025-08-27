#!/bin/sh
# shellcheck disable=SC3037
# shellcheck disable=SC2154

command_registry_get() {
  if ! registry_host=$(read_ansible_var_value "registry" 2>&1); then
    echo -n "$registry_host"
    return 1
  fi

  if ! registry_path=$(read_ansible_var_value "registry_path" 2>&1); then
    echo -n "$registry_path"
    return 1
  fi

  if ! docker_file=$(host cat ~/.docker/config.json 2>&1); then
    docker_file="{}"
  fi

  echo -n "$registry_host*$registry_path*$docker_file" > "$1"
}
