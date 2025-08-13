#!/bin/bash

configs_path=$(dirname $0)
. "$configs_path/render_common.sh"
export_defaults_from_env_file radariq.env

add_sensor() {
  if [ -n "$2" ]; then
    IFS=':' read -r user pass <<< "$1"
    shift
  else
    user=$RADARIQ_USERNAME
    pass=$RADARIQ_PASSWORD
  fi

  IFS='#' read -r host_ports_camera_distance camera_model radar_model <<< "$1"
  [ -z "$camera_model" ] && camera_model=$RADARIQ_CAMERA_MODEL

  IFS='^' read -r host_ports camera_distance <<< "$host_ports_camera_distance"
  [ -z "$camera_distance" ] && camera_distance=$RADARIQ_CAMERA_DISTANCE

  IFS=':' read -r host http_port ws_port <<< "$host_ports"
  [ -z "$http_port" ] && http_port=$RADARIQ_HTTP_PORT
  [ -z "$ws_port" ] && ws_port=$RADARIQ_WS_PORT

  cat <<EOF
  - radariq:
      http:
        host: $host
        port: $http_port
      timeout: $RADARIQ_HTTP_TIMEOUT
      update_interval: $RADARIQ_UPDATE_INTERVAL
      websocket:
        host: $host
        port: $ws_port
      auth:
        username: $user
        password: $pass
      camera_model: "${camera_model//_/ }"
      camera_distance: $camera_distance
EOF
  [ -n "$radar_model" ] && cat <<EOF
      radar_model: "${radar_model//_/ }"
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
