package wsclient

import (
	"dds-provider/internal/core"
	"fmt"
	"strings"
)

var wsclientError = core.ProviderError()

var (
	ErrWSDialTimeout        = wsclientError("websocket dial timeout")
	ErrWSConnectionRefused  = wsclientError("websocket connection refused")
	ErrWSConnectionFailed   = wsclientError("websocket connection failed")
	ErrWSReadFailed         = wsclientError("websocket read failed")
	ErrWSUnmarshalFailed    = wsclientError("websocket unmarshal failed")
	ErrWSNotificationFailed = wsclientError("websocket notification failed")
)

func handleWebsocketError(operation, url string, err error) error {
	if err == nil {
		return nil
	}

	errStr := err.Error()
	switch {
	case strings.Contains(errStr, "connection refused"):
		return wrapWithContext(ErrWSConnectionRefused, operation, url, err)
	case strings.Contains(errStr, "timeout"):
		return wrapWithContext(ErrWSDialTimeout, operation, url, err)
	default:
		return wrapWithContext(ErrWSConnectionFailed, operation, url, err)
	}
}

func handleReadError(operation, url string, err error) error {
	if err == nil {
		return nil
	}

	return wrapWithContext(ErrWSReadFailed, operation, url, err)
}

func handleUnmarshalError(operation, url string, err error) error {
	if err == nil {
		return nil
	}

	return wrapWithContext(ErrWSUnmarshalFailed, operation, url, err)
}

func handleNotificationError(operation, url string, err error) error {
	if err == nil {
		return nil
	}

	return wrapWithContext(ErrWSNotificationFailed, operation, url, err)
}

func wrapWithContext(categorizedErr error, operation, url string, originalErr error) error {
	if categorizedErr != nil {
		return categorizedErr
	}

	context := operation
	if url != "" {
		context = fmt.Sprintf("%s %s %s", context, " for url ", url)
	}

	return wsclientError("%s failed: %v", context, originalErr)
}
