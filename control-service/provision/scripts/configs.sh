#!/bin/sh

command_read_devices_config() {
  tee -a "$1" < "$CONFIGS_DIR/devices.env"
}

command_update_devices_config() {
  shift

  echo -n "$@" | tee "$CONFIGS_DIR/devices.env"

  for service in dds-target-provider dds-camera-api dds-camera-video; do
    if ! redeploy_service $service; then
      return 1
    fi
  done
}

command_read_system_config() {
  tee -a "$1" < "$CONFIGS_DIR/system.yaml"
}

command_update_system_config() {
  shift

  echo -n "$@" | tee "$CONFIGS_DIR/system.yaml"

  touch "$TRIGGERS_DIR/pipeline_update_system_config"
  if log=$(redeploy_service dds-control-update 2>&1); then
    sleep 20
  fi
  rm "$TRIGGERS_DIR/pipeline_update_system_config"
  echo "Self restart timeout, log:"
  echo -n "$log"
  return 1
}

pipeline_update_system_config() {
  touch "$TRIGGERS_DIR/result_ok_command_update_system_config"
}
