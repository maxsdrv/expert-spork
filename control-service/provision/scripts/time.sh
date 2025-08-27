#!/bin/sh
# shellcheck disable=SC3037

command_ntp_set() {
  ntp_servers="$2"

  run_playbook update-ntp.yml --extra-vars ntp_servers="\"$ntp_servers\""
}

command_ntp_get() {
  if ! ntp_info=$(host timedatectl show-timesync); then
    return 1
  fi

  ntp_servers=$(echo "$ntp_info" | grep '^SystemNTPServers=' | cut -d'=' -f2)

  echo -n "$ntp_servers" > "$1"
}

command_get_ntp_sync() {
  if ! ntp_state=$(host timedatectl -p NTP --value show); then
    return 1
  fi
  
  echo -n "$ntp_state" > "$1"
}

command_set_ntp_sync() {
  host timedatectl set-ntp "$2"
}

command_set_time() {
  formatted_time=$(host date -d "$2" -u +"%Y-%m-%d%H:%M:%S")

  if host timedatectl set-time "$formatted_time" ; then
    echo -n "ok" > "$1"
  else
    echo -n "error" > "$1"
  fi
}

command_get_time() {
  if ! current_time=$(host date -u +"%Y-%m-%dT%H:%M:%SZ"); then
    return 1
  fi

  echo -n "$current_time" > "$1"
}
