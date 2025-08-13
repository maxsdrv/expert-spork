#!/bin/bash

configs_path=$(dirname $0)
. "$configs_path/render_common.sh"
export_defaults_from_env_file tamerlan.env

add_channel() {
  cat <<EOF
        - number: $1
          name: "$2"
EOF
}

add_jammer() {
  IFS='#' read -r channels_modbus_distance model <<< "$1"
  IFS='^' read -r channels_modbus distance <<< "$channels_modbus_distance"
  [ -z "$distance" ] && distance=$TAMERLAN_JAMMERS_DISTANCE
  IFS='@' read -r channels modbus <<< "$channels_modbus"
  [ -z "$modbus" ] && modbus=$TAMERLAN_JAMMER_ADDRESS

  cat <<EOF
      - connection: connection_1
        sensor: 1
        modbus_address: $modbus
        update_interval: $TAMERLAN_JAMMERS_INTERVAL
        distance: $distance
EOF
  [ -n "$model" ] && cat <<EOF
        model: "${model//_/ }"
EOF
  [ -n "$2" ] && cat <<EOF
        group_id: $2
EOF

  cat <<EOF
        channels:
EOF
  number=0
  echo "$channels" | tr ":" "\n" | \
  while read -r name; do
    number=$((number+1))
    [ -z "$name" ] && continue

    # shellcheck disable=SC2046
    add_channel "$number" "$name"
  done
}

add_sensor() {
  if [ -n "$2" ]; then
    connection_detection_count_model=$2
    modbus=$1
  else
    connection_detection_count_model=$1
    modbus=$TAMERLAN_SENSOR_ADDRESS
  fi

  IFS='#' read -r connection_detection_count model <<< "$connection_detection_count_model"
  IFS='~' read -r connection detection_count <<< "$connection_detection_count"
  [ -z "$detection_count" ] && detection_count=$TAMERLAN_DETECTION_COUNT

  if [ "${connection::1}" = "/" ]; then
    IFS=: read -r tty speed <<< "$connection"
    [ -z "$speed" ] && speed=$TAMERLAN_RTU_SPEED

    cat <<EOF
  - tamerlan:
      connections:
      - id: connection_1
        type: rtu
        name: $tty
        baud_rate: $speed
        parity: N
        data_bits: 8
        stop_bits: 1
        timeout: $TAMERLAN_CONNECTION_TIMEOUT
EOF
  else
    IFS=: read -r host port <<< "$connection"
    [ -z "$port" ] && port=$TAMERLAN_TCP_PORT

    cat <<EOF
  - tamerlan:
      connections:
      - id: connection_1
        type: tcp
        host: $host
        port: $port
        timeout: $TAMERLAN_CONNECTION_TIMEOUT
EOF
  fi

  cat <<EOF
      sensors:
      - connection: connection_1
        number: 1
        modbus_address: $modbus
        sensor_update_interval: $TAMERLAN_SENSOR_INTERVAL
        objects_update_interval: $TAMERLAN_OBJECTS_INTERVAL
        min_detection_count: $detection_count
EOF
  [ -n "$model" ] && cat <<EOF
        model: "${model//_/ }"
EOF
}

add_device() {
  # shellcheck disable=SC2046
  add_sensor $(echo "$1" | tr "@" " ")
  shift
  [ -n "$*" ] && cat <<EOF
      jammers:
EOF
  for jammer in "$@"; do
    # shellcheck disable=SC2046
    add_jammer $(echo "$jammer" | tr "*" " ")
  done
}

for device in "$@"; do
  # shellcheck disable=SC2046
  add_device $(echo "$device" | tr "|" " ")
done
