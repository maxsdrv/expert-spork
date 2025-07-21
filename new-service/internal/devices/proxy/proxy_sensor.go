package proxy

import "dds-provider/internal/core"

type ProxySensor struct {
	//api         *dss_target_service.SensorAPIService
}

func NewProxySensor( /*api *dss_target_service.SensorAPIService*/ ) *ProxySensor {
	return &ProxySensor{
		//		api: api,
	}
}

func (s *ProxySensor) SetJammerMode(mode core.JammerMode) error {
	return nil
}
