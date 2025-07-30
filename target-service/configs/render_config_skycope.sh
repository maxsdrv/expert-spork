#!/bin/bash

configs_path=$(dirname $0)

# shellcheck disable=SC2046
export $(cat "$configs_path/defaults/skycope.env")

add_jammer() {
  IFS='#' read -r dummy_distance model <<< "$1"
  [ -z "$model" ] && model=$SKYCOPE_JAMMER_MODEL
  IFS='^' read -r _ distance <<< "$dummy_distance"
  [ -z "$distance" ] && distance=$SKYCOPE_JAMMER_DISTANCE

  cat <<EOF
      jammer_distance: $distance
      jammer_model: "${model//_/ }"
EOF
}

add_sensor() {
  if [ -n "$2" ]; then
    IFS=':' read -r user pass <<< "$1"
    shift
  else
    user=$SKYCOPE_USERNAME
    pass=$SKYCOPE_PASSWORD
  fi

  IFS='#' read -r host_port model <<< "$1"
  [ -z "$model" ] && model=$SKYCOPE_MODEL

  IFS=: read -r host port <<< "$host_port"
  [ -z "$port" ] && port=$SKYCOPE_PORT

  cat <<EOF
  - skycope:
      http:
        host: $host
        port: $port
      timeout: $SKYCOPE_HTTP_TIMEOUT
      update_interval: $SKYCOPE_UPDATE_INTERVAL
      auth:
        username: $user
        password: $pass
      model: "${model//_/ }"
EOF
}

add_device() {
  # shellcheck disable=SC2046
  add_sensor $(echo "$1" | tr "@" " ")
  add_jammer "$2"
}

for device in "$@"; do
  # shellcheck disable=SC2046
  add_device $(echo "$device" | tr "|" " ")
done
