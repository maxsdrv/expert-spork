#!/bin/sh
# shellcheck disable=SC3037
# shellcheck disable=SC3001

command_system_reboot() {
  echo "System reboot start" | tee -a "$1"
  rm -f "$TRIGGERS_DIR/status_reboot_"*

  touch "$TRIGGERS_DIR/pipeline_system_reboot"

  if log=$(host reboot 2>&1); then
    sleep 20
  fi

  echo -n "$log" | tee "$TRIGGERS_DIR/status_reboot_error"
  rm -f "$TRIGGERS_DIR/pipeline_system_reboot"
  return 1
}

pipeline_system_reboot() {
  echo "Reboot process finished successfully" | tee -a "$1"

  touch "$TRIGGERS_DIR/status_reboot_ok"
}

command_system_shutdown() {
  echo "System shutdown start" | tee -a "$1"

  if ! log=$(host shutdown now); then
    echo -n "$log" | tee -a "$1"
    return 1
  fi

  echo -n "$log" | tee -a "$1"
  return 0
}
