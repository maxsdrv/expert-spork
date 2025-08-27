package config

var Metrics struct {
	NodeExporterHost string
}

func init() {
	load(&Metrics)
}
