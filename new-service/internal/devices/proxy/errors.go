package proxy

import (
	"dds-provider/internal/core"
)

var proxyError = core.ProviderErrorFn("proxy")

var (
	ErrTimeout            = proxyError("timeout")
	ErrConnectionRefused  = proxyError("connection refused")
	ErrConnectionReset    = proxyError("connection reset")
	ErrNetworkUnreachable = proxyError("network unreachable")
	ErrHostNotFound       = proxyError("host not found")

	ErrHTTPTimeout      = proxyError("http timeout")
	ErrHTTPServerError  = proxyError("http server error")
	ErrHTTPUnauthorized = proxyError("http unauthorized")
	ErrHTTPBadStatus    = proxyError("http bad status")

	ErrServiceUnavailable = proxyError("service unavailable")
	ErrDeviceNotFound     = proxyError("device not found")
	ErrInvalidDeviceType  = proxyError("invalid device type")

	ErrInvalidMessageFormat = proxyError("invalid message format")
	ErrMarshallingFailed    = proxyError("marshalling failed")
	ErrUnmarshallingFailed  = proxyError("unmarshalling failed")
	ErrDataValidationFailed = proxyError("data validation failed")
)
