#!/bin/sh
# shellcheck disable=SC3037
# shellcheck disable=SC3001

self_update_finish() {
  version="$SELF_UPDATE_VERSION"
  echo "Self update finishing, version: '$version'"
  if [ -e "$TRIGGERS_DIR/pipeline_update_services" ]; then
    read -r online version service < "$TRIGGERS_DIR/pipeline_update_services"
    echo "Services update have been already scheduled, online: '$online', version: '$version', service: '$service'"
  else
    if [ -e "$TRIGGERS_DIR/pipeline_update_rollback_services" ]; then
      echo "Services rollback have been already scheduled"
    else
      if [ -e "$TRIGGERS_DIR/pipeline_update_system_config" ]; then
        echo "System config update have been already scheduled"
      else
        echo -n "false $version" > "$TRIGGERS_DIR/pipeline_update_services"
        echo "Scheduled all services update from firmware"
      fi
    fi
  fi
  if ! log=$(run_playbook update-services.yml --tags dds-control-update --extra-vars "image_tag=$version" 2>&1); then
    rm -f "$TRIGGERS_DIR/pipeline_update_services"
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
    exit 1
  fi
  echo "Self update finished:"
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_self_ok"
  exit 0
}
[ -n "$SELF_UPDATE_VERSION" ] && self_update_finish

command_update_firmware() {
  echo "Update firmware start" | tee -a "$1"
  rm -f "$TRIGGERS_DIR/status_update_"*

  echo "Backup start" | tee -a "$1"
  touch "$TRIGGERS_DIR/status_update_backup"
  # shellcheck disable=SC2154
  if ! log=$(run_playbook backup.yml --extra-vars "services='$services_list'" 2>&1); then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
    return 1
  fi

  if log=$(run_playbook update-self-firmware.yml 2>&1); then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_self_started"

    sleep 10
  fi
  echo "Failed waiting for self update" | tee "$TRIGGERS_DIR/status_update_error"
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
  return 1
}

command_update_online() {
  read -r version service < <(echo -n "$2")
  echo "Update online start, version: '$version', service: '$service'" | tee -a "$1"
  rm -f "$TRIGGERS_DIR/status_update_"*

  # shellcheck disable=SC2154
  [ -n "$service" ] && services="$service" || services="$services_list"
  echo "Backup start, services: '$services'" | tee -a "$1"
  touch "$TRIGGERS_DIR/status_update_backup"
  if ! log=$(run_playbook backup.yml --extra-vars "services='$services'" 2>&1); then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
    return 1
  fi

  [ "$version" = "local" ] && online=false || online=true
  echo -n "$online $version $service" > "$TRIGGERS_DIR/pipeline_update_services"

  if [ -n "$service" ] && [ "$service" != "dds-control-update" ]; then
    echo "Self update skipped:" | tee -a "$1"
    echo "$log" | tee -a "$1"
    return 0
  fi

  if log=$(run_playbook update-self-online.yml --extra-vars "{ online: $online }" --extra-vars "image_tag=$version" 2>&1); then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_self_started"

    sleep 10
  fi
  rm "$TRIGGERS_DIR/pipeline_update_services"
  echo "Failed waiting for self update" | tee "$TRIGGERS_DIR/status_update_error"
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
  return 1
}

command_update_rollback() {
  echo "Rollback start" | tee -a "$1"
  rm -f "$TRIGGERS_DIR/status_update_"*

  if ! log=$(run_playbook rollback-self.yml 2>&1); then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
    return 1
  fi

  touch "$TRIGGERS_DIR/pipeline_update_rollback_services"

  if echo "$log" | grep -q dds-control-update; then
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_self_started"

    sleep 10
  else
    echo "Self update skipped:" | tee -a "$1"
    echo "$log" | tee -a "$1"
    return 0
  fi

  rm "$TRIGGERS_DIR/pipeline_update_rollback_services"
  echo "Failed waiting for self update" | tee "$TRIGGERS_DIR/status_update_error"
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
  return 1
}

pipeline_update_services() {
  read -r online version service < <(echo -n "$2")
  echo "Services update start, online: '$online', version: '$version', service: '$service'" | tee -a "$1"

  if [ -z "$service" ]; then
    tags="--skip-tags dds-control-update"
  elif [ "$service" != "dds-control-update" ]; then
    tags="--tags $service"
  else
    echo "Services update skipped" | tee -a "$1"
    touch "$TRIGGERS_DIR/status_update_ok"
    return 0
  fi

  if log=$(run_playbook_with_cancel update-services.yml $tags --extra-vars "{ online: $online }" --extra-vars "image_tag=$version" 2>&1); then
    echo "Services update done:"
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_ok"
    return 0
  fi
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
  return 1
}

pipeline_update_rollback_services() {
  echo "Services rollback start" | tee -a "$1"

  if log=$(run_playbook_with_cancel rollback-services.yml --skip-tags dds-control-update 2>&1); then
    echo "Services update done:"
    echo -n "$log" | tee "$TRIGGERS_DIR/status_update_ok"
    return 0
  fi
  echo -n "$log" | tee "$TRIGGERS_DIR/status_update_error"
  return 1
}
