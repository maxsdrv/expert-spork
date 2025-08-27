#!/bin/sh
# shellcheck disable=SC3037

command_system_restore() {
  echo "System restore start" | tee -a "$1"

  # shellcheck disable=SC2154
  restore_dir="$data_root/backups/restore"
  backup_file=$(host find "$restore_dir" -maxdepth 1 -type f | head -n 1)
  if ! host tar -xJf "$backup_file" -C "$restore_dir"; then
    return 1
  fi
  if ! host duplicity restore --no-encryption --force "file://$restore_dir" /; then
    return 1
  fi
  if ! host sh -c "rm -rf \"$restore_dir\"/*"; then
    return 1
  fi
  if ! host systemctl restart docker; then
    return 1
  fi
  return 0
}
