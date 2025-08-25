#!/bin/sh

set -e

configs_path=$(dirname "$0")

. "$configs_path"/dev.env

for device in $BULAT_DEVICES; do
  IFS='@' read -r BULAT_TOKEN BULAT_HOST << EOF
$device
EOF

export BULAT_TOKEN
export BULAT_HOST

done
