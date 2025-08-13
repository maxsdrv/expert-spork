#!/usr/bin/env bash

configs_path=$(dirname $0)/configs
$configs_path/render_config.sh dss-target-service.yaml

sed -i -e "s/DEBUG/$LOG_LEVEL/g;s/INFO/$LOG_LEVEL/g" dss-target-service.log.ini

code=99
while [ "$code" = "99" ]; do
  "$@"
  code=$?
done
