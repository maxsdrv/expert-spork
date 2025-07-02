package config

var General struct {
	Listen string
}

func init() {
	load(&General)
}
