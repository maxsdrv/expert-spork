package core

import (
	"fmt"
	"runtime"
	"strings"
)

func ProviderErrorFn(module string) func(string, ...any) error {
	return func(format string, args ...any) error {
		return fmt.Errorf(module+": "+format, args...)
	}
}

var coreError = ProviderErrorFn("core")

func ProviderError() func(string, ...any) error {
	return func(format string, args ...any) error {
		pc, _, _, _ := runtime.Caller(1)
		funcName := runtime.FuncForPC(pc).Name()

		var packageName string
		if idx := strings.LastIndex(funcName, "."); idx != -1 {
			packagePath := funcName[:idx]

			if strings.Contains(packagePath, ")") {
				if lastSlash := strings.LastIndex(packagePath, "/"); lastSlash != -1 {
					packageName = packagePath[lastSlash+1:]
					if dotIdx := strings.Index(packageName, "."); dotIdx != -1 {
						packageName = packageName[:dotIdx]
					}
				}
			} else {
				if pkgIdx := strings.LastIndex(packagePath, "/"); pkgIdx != -1 {
					packageName = packagePath[pkgIdx+1:]
				} else {
					packageName = packagePath
				}
			}
		} else {
			packageName = "unknown"
		}

		packageError := ProviderErrorFn(packageName)

		return packageError(format, args...)
	}
}
