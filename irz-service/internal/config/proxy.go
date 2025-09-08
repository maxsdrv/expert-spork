package config

var Proxy struct {
	Host string
}

func init() {
	load(&Proxy)
}
