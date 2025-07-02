package config

var General struct {
	Listen   string
	DbPgName string
	DbPgUser string
	DbPgPass string
	DbPgHost string
	DbPgPort string
	LogLevel string
}

func init() {
	load(&General)
}
