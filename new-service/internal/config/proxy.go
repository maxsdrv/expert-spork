package config

var ProxyConfig struct {
	Host     string
	PortHttp int
	PortWs   int
}

func init() {
	load(&ProxyConfig)
}
