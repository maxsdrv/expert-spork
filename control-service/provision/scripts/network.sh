#!/bin/sh
# shellcheck disable=SC3037

if ! net_interface=$(read_ansible_var_value "net_if" 2>&1); then
  echo "Failed to get net interface:"
  echo "$net_interface"
  exit 1
fi

command_get_network_settings() {
  # Getting address and prefix
  if ! addr_data=$(host ip -o -f inet addr show "$net_interface" | xargs | cut -d " " -f4); then
    return 1
  fi

  # Getting gateway
  if ! gateway=$(host ip route show to default dev "$net_interface" | cut -d " " -f3); then
    return 1
  fi
  if [ -z "$gateway" ]; then
    gateway="-"
  fi

  # Getting dns-nameservers
  if ! dns_nameservers="$(host resolvectl dns "$net_interface" | cut -d ":" -f2)"; then
    return 1
  fi

  echo "$addr_data $gateway $dns_nameservers" > "$1"
}

command_get_netplan_config_file() {
  for file in $(host ls /etc/netplan); do
    if host grep $net_interface: /etc/netplan/$file > /dev/null; then
      echo "$net_interface $file $(host cat /etc/netplan/$file)" > "$1"
      return
    fi
  done

  file_template="network:
  version: 2
  ethernets:
    $net_interface:
      addresses: []"
  echo "$net_interface 01-netcfg.yaml $file_template" > "$1"
}

command_set_netplan_config_file() {
  file_name=$(echo "$2" | awk 'NR==1 {print;}')
  file_content=$(echo "$2" | awk 'NR>1 {print;}')

  if [ -z "$file_name" ]; then
    echo "Missing file name"
    return 1
  fi

  if [ -z "$file_content" ]; then
    echo "Missing file content"
    return 1
  fi

  netconfig_save()
  {
    if host ls "/etc/netplan/$1" 2>/dev/null; then
      host mv -f "/etc/netplan/$1" "/etc/netplan/$1.bak"
    else
      host rm -f "/etc/netplan/$1.bak"
    fi
  }

  netconfig_restore()
  {
    if host ls "/etc/netplan/$1.bak" 2>/dev/null; then
      host mv -f "/etc/netplan/$1.bak" "/etc/netplan/$1"
    else
      host rm -f "/etc/netplan/$1"
    fi
  }

  netconfig_prepare()
  {
    host cat << EOF | host tee "/etc/netplan/$1"
$2
EOF
  }

  netconfig_save "$file_name"
  netconfig_prepare "$file_name" "$file_content"

  if ! host netplan apply; then
    netconfig_restore "$file_name"
    echo "Can't configure $net_interface net interface"
    return 1
  fi

  redeploy_service dds-camera-video
}
