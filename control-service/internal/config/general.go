package config

var General struct {
	ListenHost string
	ListenPort string
}

func init() {
	load(&General)
}
