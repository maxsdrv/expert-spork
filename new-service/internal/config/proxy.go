package config

import (
	"dds-provider/internal/devices/proxy"
	"log"
	"strconv"
	"strings"
)

const (
	DefaultProviderPortHttp = 19080
	DefaultPortWs           = 19081
)

var Proxy struct {
	Devices []string `envSeparator:" "`
}

var TargetProviderConnections []proxy.Connection

func init() {
	load(&Proxy)

	for _, device := range Proxy.Devices {
		connection := parseConnection(device)
		TargetProviderConnections = append(TargetProviderConnections, connection)
	}
}

func parseConnection(device string) proxy.Connection {
	var connection proxy.Connection

	entry := strings.SplitN(device, "#", 2)[0]
	hostParts := strings.Split(entry, ":")
	connection.Host = hostParts[0]

	if len(hostParts) == 2 {
		connection.PortHttp = DefaultProviderPortHttp
		connection.PortWs = DefaultPortWs
	} else if len(hostParts) == 2 {
		httpPort, err := strconv.Atoi(hostParts[1])
		if err != nil {
			log.Fatalf("Invalid HTTP port format: %s", hostParts[1])
		}
		connection.PortHttp = httpPort
		connection.PortWs = DefaultPortWs
	} else {
		httpPort, err := strconv.Atoi(hostParts[1])
		if err != nil {
			log.Fatalf("Invalid HTTP port format: %s", hostParts[1])
		}
		wsPort, err := strconv.Atoi(hostParts[2])
		if err != nil {
			log.Fatalf("Invalid WS port format: %s", hostParts[2])
		}
		connection.PortHttp = httpPort
		connection.PortWs = wsPort
	}

	return connection
}
