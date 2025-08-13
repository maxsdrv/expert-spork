#!/bin/bash

configs_path=$(dirname $0)
. "$configs_path/render_common.sh"
export_defaults_from_env_file generic.env

add_jammer() {
  echo Jammers not implemented && exit 1
}

add_sensor() {
IFS=: read -r host http_port ws_port <<< "$1"

cat <<EOF
  - rest:
      sensors:
        - model: SENSOR_TYPE_1
          http:
            host: $host
            port: $http_port
          timeout: $GENERIC_CONNECTION_TIMEOUT
          update_interval: $GENERIC_SENSOR_INTERVAL
          websocket:
            host: $host
            port: $ws_port
EOF
}

add_device() {
  # shellcheck disable=SC2046
  add_sensor $(echo "$1" | tr "@" " ")
  shift
  [ -n "$*" ] && cat <<EOF
      Jammers_not_implemented:
EOF
  for jammer in "$@"; do
    # shellcheck disable=SC2046
    add_jammer $(echo "$jammer" | tr "@" " ")
  done
}

for device in "$@"; do
  # shellcheck disable=SC2046
  add_device $(echo "$device" | tr "|" " ")
done
