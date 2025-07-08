package proxy

import (
	"dds-provider/internal/core"
	"errors"
	"fmt"
)

type ProxyJammer struct {
}

func (j *ProxyJammer) SetJammerBands(bands core.JammerBands, duration int32) error {
	if 0 == duration {
		// suppose local library error happened
		err := errors.New("local error")
		return fmt.Errorf("bridge: %v", err)
	}
	if 1 == duration {
		//suppose bridge operation related network error happened
		return &ProxyError{Op: "websocket reply", Err: errors.New("timed out")}
	}

	return nil
}
