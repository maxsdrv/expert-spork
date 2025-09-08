package core

import (
	"fmt"
)

func ProviderErrorFn(module string) func(string, ...any) error {
	return func(format string, args ...any) error {
		return fmt.Errorf(module+": "+format, args...)
	}
}

var coreError = ProviderErrorFn("core")
