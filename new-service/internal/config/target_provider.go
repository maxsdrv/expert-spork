package config

import (
	"log"
	"strconv"
	"strings"

	"dds-provider/internal/services/bridge"
)

const (
	DefaultProviderPortHttp = 19080
	DefaultPortWs           = 19081
)

var targetProvider struct {
	devices []string `envSeparator:" "`
}

var TargetProviderConnections []bridge.Connection

func init() {
	load(&targetProvider)

	for _, device := range targetProvider.devices {
		connection := parseConnection(device)
		TargetProviderConnections = append(TargetProviderConnections, connection)
	}
}

func parseConnection(device string) bridge.Connection {
	var connection bridge.Connection

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
