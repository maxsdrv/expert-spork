package config

import (
	"log"
	"strconv"
	"strings"

	"dds-provider/proxy/internal/services/target_provider"
)

const (
	DefaultPortHttp = 19080
	DefaultPortWs   = 19081
)

var proxy struct {
	TargetProviderDevices []string `envSeparator:" "`
}

var TargetProviderConnections []target_provider.Connection

func init() {
	load(&proxy)

	for _, device := range proxy.TargetProviderDevices {
		connection := parseConnection(device)
		TargetProviderConnections = append(TargetProviderConnections, connection)
	}
}

func parseConnection(device string) target_provider.Connection {
	var connection target_provider.Connection

	entry := strings.SplitN(device, "#", 2)[0]
	hostParts := strings.Split(entry, ":")
	connection.Host = hostParts[0]

	if len(hostParts) == 2 {
		connection.PortHttp = DefaultPortHttp
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
