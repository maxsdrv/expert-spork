package config

import (
// "dds-provider/internal/devices/bulat"
)

const (
	DefaultUser     = "admin"
	DefaultPassword = "admin"
	DefaultPortHttp = 80
)

var bulatVar struct {
	BulatDevices []string `envSeparator:" "`
}

func init() {
	load(&bulatVar)
	for _, _ = range bulatVar.BulatDevices {
	}
}

func parseSomething(input string) {
}
