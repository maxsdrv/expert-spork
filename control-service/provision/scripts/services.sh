#!/bin/sh
# shellcheck disable=SC3037

if ! services_config=$(read_ansible_var_object "services" 2>&1); then
  echo "Failed to get services config:"
  echo "$services_config"
  exit 1
fi

command_services_config() {
  echo "$services_config" > "$1"
}

command_services_list() {
  # shellcheck disable=SC2154
  for container in $services_list; do
    format="- {service: $container, info: {state: {{json .State.Status }}, version: {{json .Config.Image }}, revision: {{index .Config.Labels \"org.opencontainers.image.version\"}}, since: {{json .State.StartedAt }}, failures: {{json .RestartCount }} } }"
    if info=$(host docker inspect "$container" --format "$format"); then
      echo "$info" >> "$1"
    else
      echo "- {service: $container, info: null}" >> "$1"
    fi
  done
}

command_services_start() {
  host docker start "$2" > "$1"
}

command_services_restart() {
  host docker restart "$2" > "$1"
}

command_services_stop() {
  host docker stop "$2" > "$1"
}
