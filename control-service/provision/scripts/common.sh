#!/bin/sh
# shellcheck disable=SC3037

alias host="nsenter -t 1 -m -u -n -i"

run_playbook_with_cancel() {
  playbook="$1"
  shift
  echo "Running cancellable playbook $playbook with args: $*"
  rm -f "$TRIGGERS_DIR/handle_update_cancel"*
  ansible-playbook "$playbook" "$@" -i inventory/production --connection=community.docker.nsenter &
  pid=$!
  while kill -0 $pid 2>/dev/null; do
    sleep 1
    if [ -f "$TRIGGERS_DIR/handle_update_cancel" ]; then
      if ! log=$(kill $pid 2>&1); then
        echo "Cancel update failed: $log"
        echo -n "$log" > "$TRIGGERS_DIR/handle_update_cancel_error"
        return
      fi

      touch "$TRIGGERS_DIR/handle_update_cancel_ok"
      return
    fi
  done
  wait $pid
}

run_playbook() {
  playbook="$1"
  shift
  echo "Running playbook $playbook with args: $*"
  ansible-playbook "$playbook" "$@" -i inventory/production --connection=community.docker.nsenter
}

redeploy_service() {
  if ! version=$(host docker inspect "$1" --format "{{json .Config.Image }}" | sed -e's/.*:\(.*\)\"/\1/' 2>&1); then
    echo -n "$version"
    return 1
  fi

  echo "Redeploying service $1 version: $version"
  if [ "$1" = "dds-control-update" ]; then
    if log=$(run_playbook update-self-online.yml --extra-vars "image_tag=$version" 2>&1); then
      return
    fi
  else
    if log=$(run_playbook update-services.yml --tags "$1" --extra-vars "image_tag=$version" 2>&1); then
      return
    fi
  fi

  echo -n "$log"
  return 1
}

read_ansible_var_object() {
  ansible all -i inventory/production -e ansible_host=localhost -m debug -a "msg={{ $1 }}" -o | sed -n "s/.*\"msg\": \(.*\)}/\1/p"
}

read_ansible_var_value() {
  read_ansible_var_object "$1" | sed -n 's/"\(.*\)"/\1/p'
}


if ! services_list=$(ansible-playbook update-services.yml -i localhost, --list-tags 2>&1); then
  echo "Failed to get services list:"
  echo "$services_list"
  exit 1
fi
services_list=$(echo "$services_list" | grep "TASK TAGS" | sed -e's/.*\[\(.*\)\]/\1/' -e's/,//g' -e's/ /\n/g' | grep "dds-" | sort -r | xargs)

if ! data_root=$(read_ansible_var_value "data_root" 2>&1); then
  echo "Failed to get data root:"
  echo "$data_root"
  exit 1
fi
