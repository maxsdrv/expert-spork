#!/bin/sh
# shellcheck disable=SC3040
set -o pipefail
# shellcheck disable=SC3037

ln -sf "$CONFIGS_DIR/system.yaml" "/provision/group_vars/production.yml"

scripts=$(dirname "$0")
. "$scripts/common.sh"
. "$scripts/update.sh"
. "$scripts/services.sh"
. "$scripts/time.sh"
. "$scripts/backup.sh"
. "$scripts/system.sh"
. "$scripts/configs.sh"
. "$scripts/network.sh"
. "$scripts/registry.sh"

echo "Service version $VERSION started with settings:"
echo "  services: $services_list"
echo "  network interface: $net_interface"
echo "  data root: $data_root"

while true; do
  for command in "$TRIGGERS_DIR/pipeline_"*; do
    [ -f "$command" ] || break # handle the case of no files

    command=$(basename "$command")
    param=$(cat "$TRIGGERS_DIR/$command")
    echo "Pipeline $command triggered: $param"
    rm "$TRIGGERS_DIR/$command"
    echo -n > "$TRIGGERS_DIR/result_$command"
    if log=$($command "$TRIGGERS_DIR/result_$command" "$param" 2>&1); then
      echo "Pipeline $command successful:"
    else
      echo "Pipeline $command failed:"
    fi
    echo "$log"
  done

  for command in "$TRIGGERS_DIR/command_"*; do
    [ -f "$command" ] || break # handle the case of no files

    command=$(basename "$command")
    param=$(cat "$TRIGGERS_DIR/$command")
    echo "Command $command triggered:"
    echo "$param"
    rm "$TRIGGERS_DIR/$command"
    echo -n > "$TRIGGERS_DIR/result_$command"
    if log=$($command "$TRIGGERS_DIR/result_$command" "$param" 2>&1); then
      echo "Command $command successful:"
      cat "$TRIGGERS_DIR/result_$command"
      echo
      mv "$TRIGGERS_DIR/result_$command" "$TRIGGERS_DIR/result_ok_$command"
    else
      echo "Command $command failed:"
      echo "$log"
      echo -n "$log" > "$TRIGGERS_DIR/result_error_$command"
    fi
  done

  sleep 1
done
