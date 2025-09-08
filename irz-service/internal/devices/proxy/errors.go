package proxy

import (
	"dds-provider/internal/core"
)

var proxyError = core.ProviderErrorFn("proxy")

var (
	ErrProxyTimeout = proxyError("timeout")
	//add more
)
