#!/bin/sh

[ -z "$LICENSE_KEY" ] && echo "LICENSE_KEY env var is required" && exit 1
public_key_h="license/dss-target-provider/include/licensecc/dss-target-provider/public_key.h"

key_info=$(echo "$LICENSE_KEY" | openssl rsa -in /dev/stdin -text -noout)
key_info=${key_info#*modulus:}
key_info=${key_info%publicExponent:*}
modulus_hex=$(echo "$key_info" | sed 's/:/ /g')

for val in $modulus_hex; do
  MODULUS="${MODULUS}0x$val,"
done

export MODULUS
LEN=$(echo "$modulus_hex" | wc -w)
export LEN=$(($LEN + 11))

envsubst < $(dirname "$0")/public_key_template.h > $public_key_h
