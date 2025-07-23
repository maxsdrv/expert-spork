package proxy

import (
	"dds-provider/internal/core"
	"errors"
	"fmt"
)

type ProxyJammer struct {
	//api         *dss_target_service.JammerAPIService
}

func NewProxyJammer( /*api *dss_target_service.JammerAPIService*/ ) *ProxyJammer {
	return &ProxyJammer{
		//		api: api,
	}
}

func (j *ProxyJammer) SetDisabled(disabled bool) {
}

func (j *ProxyJammer) SetPosition(position core.GeoPosition) error {
	return nil
}

func (j *ProxyJammer) SetPositionMode(mode core.GeoPositionMode) error {
	return nil
}

func (j *ProxyJammer) SetJammerBands(bands core.JammerBands, duration int32) error {
	if 0 == duration {
		// suppose local library error happened
		err := errors.New("local error")
		return fmt.Errorf("proxy: %v", err)
	}
	if 1 == duration {
		//suppose proxy operation related network error happened
		return &ProxyError{Op: "websocket reply", Err: errors.New("timed out")}
	}

	return nil
}
