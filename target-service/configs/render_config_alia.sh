#!/bin/bash

configs_path=$(dirname $0)

# shellcheck disable=SC2046
export $(cat "$configs_path/defaults/alia.env")

add_channel() {
  cat <<EOF
            - coil:
                sign: $2
                address: 0${1}00
EOF
}

add_device() {
  IFS='@' read -r device_id host_port <<< "$1"
  if [ -z "$host_port" ]; then
    host_port=$device_id
    device_id=$ALIA_DEVICE_ID
  fi

  IFS=':' read -r host port <<< "$host_port"
  [ -z "$port" ] && port=$ALIA_PORT

  IFS='#' read -r channels_distance model <<< "$2"
  [ -z "$model" ] && model=$ALIA_MODEL
  IFS='^' read -r channels distance <<< "$channels_distance"
  [ -z "$distance" ] && distance=$ALIA_DISTANCE

  cat <<EOF
  - alia:
      model: "${model//_/ }"
      distance: $distance
      update_interval: $ALIA_UPDATE_INTERVAL
      controllers:
        - moxa:
            host: $host
            port: $port
          device_id: $device_id
          coils:
EOF

  number=-1
  echo "$channels" | tr ":" "\n" | \
  while read -r name; do
    number=$((number+1))
    [ -z "$name" ] && continue

    # shellcheck disable=SC2046
    add_channel "$number" "$name"
  done
}

for device in "$@"; do
  # shellcheck disable=SC2046
  add_device $(echo "$device" | tr "|" " ")
done
