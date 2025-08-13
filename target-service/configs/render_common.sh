#!/bin/bash

configs_path=$(dirname $0)

export_defaults_from_env_file() {
  # Read the environment file line by line
  while read -r line; do
    # Skip empty lines and comments
    if [[ -z "$line" ]] || [[ "${line::1}" == "#" ]]; then
      continue
    fi

    # Extract variable name assuming VAR_NAME=value format
    VAR_NAME=$(echo "$line" | cut -d'=' -f1)

    # Check if the variable is not already defined in the current environment
    if [[ -z "${!VAR_NAME}" ]]; then
      echo -n "Using default" 1>&2
      export "$line"
    else
      echo -n "Using defined" 1>&2
    fi
    echo " var value: $VAR_NAME=${!VAR_NAME}" 1>&2
  done < "$configs_path/defaults/$1"
}
