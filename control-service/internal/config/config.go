package config

import (
	"errors"

	"github.com/caarlos0/env/v11"
)

var err error

func Error() error {
	return err
}

func load(config interface{}) {
	err = errors.Join(err, env.ParseWithOptions(config, env.Options{
		UseFieldNameByDefault: true,
		RequiredIfNoDef:       true,
	}))
}
