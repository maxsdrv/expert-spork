package proxy

import (
	"errors"
	"fmt"
	"net/http"
	"strings"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

var proxyError = core.ProviderError()

var (
	ErrTimeout            = proxyError("timeout")
	ErrConnectionRefused  = proxyError("connection refused")
	ErrConnectionReset    = proxyError("connection reset")
	ErrNetworkUnreachable = proxyError("network unreachable")
	ErrHostNotFound       = proxyError("host not found")
	ErrServerError        = proxyError("http server error")
	ErrUnauthorized       = proxyError("http unauthorized")
	ErrServiceUnavailable = proxyError("service unavailable")
	ErrDeviceNotFound     = proxyError("device not found")
)

func handleProxyError(operation, deviceId string, err error) error {
	if err == nil {
		return nil
	}

	if errors.Is(err, http.ErrHandlerTimeout) {
		return ErrTimeout
	}

	errStr := err.Error()
	switch {
	case strings.Contains(errStr, "connection refused"):
		return wrapWithContext(ErrConnectionRefused, operation, deviceId, err)
	case strings.Contains(errStr, "connection reset"):
		return wrapWithContext(ErrConnectionReset, operation, deviceId, err)
	case strings.Contains(errStr, "timeout"):
		return wrapWithContext(ErrTimeout, operation, deviceId, err)
	case strings.Contains(errStr, "network unreachable"):
		return wrapWithContext(ErrNetworkUnreachable, operation, deviceId, err)
	case strings.Contains(errStr, "no such host"):
		return wrapWithContext(ErrHostNotFound, operation, deviceId, err)
	}

	var apiErr *dss_target_service.GenericOpenAPIError
	if errors.As(err, &apiErr) {
		return handleApiErrors(operation, deviceId, apiErr)
	}

	return wrapWithContext(nil, operation, deviceId, err)
}

func handleApiErrors(operation, deviceId string, apiErr *dss_target_service.GenericOpenAPIError) error {
	errBody := string(apiErr.Body())

	var categorizedErr error
	switch {
	case strings.Contains(errBody, "500"):
		categorizedErr = ErrServerError
	case strings.Contains(errBody, "401"):
		categorizedErr = ErrUnauthorized
	case strings.Contains(errBody, "404"):
		categorizedErr = ErrDeviceNotFound
	case strings.Contains(errBody, "503"):
		categorizedErr = ErrServiceUnavailable
	default:
		categorizedErr = nil
	}

	return wrapWithContext(categorizedErr, operation, deviceId, apiErr)
}

func wrapWithContext(categorizedErr error, operation, deviceId string, originalErr error) error {
	if categorizedErr != nil {
		return categorizedErr
	}

	context := operation
	if deviceId != "" {
		context = fmt.Sprintf("%s %s %s", context, " for device ", deviceId)
	}

	return proxyError("%s failed: %v", context, originalErr)
}

func handleJammerError(operation, jammerId string, err error) error {
	return handleProxyError(operation, jammerId, err)
}

func handleSensorError(operation, sensorId string, err error) error {
	return handleProxyError(operation, sensorId, err)
}
