package proxy

import (
	"context"
	"dds-provider/internal/services/mapping"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

type Sensor struct {
	id         string
	serviceAPI *dss_target_service.SensorAPIService
	info       *dss_target_service.SensorInfo
}

func NewSensor(sensorId string, api *dss_target_service.SensorAPIService, info *dss_target_service.SensorInfo) *Sensor {
	return &Sensor{
		id:         sensorId,
		serviceAPI: api,
		info:       info,
	}
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout int32) error {
	dssMode, err := mapping.ConvertJammerMode(mode)
	if err != nil {
		return err
	}

	setJammerModeReq := dss_target_service.SetJammerModeRequest{
		Id:         s.id,
		JammerMode: dssMode,
		Timeout:    timeout,
	}

	_, err = s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	return err
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	return *mapping.ConvertToAPISensorInfo(*s.info)
}

func (s *Sensor) SetDisabled(disabled bool) {
	return
}

func (s *Sensor) SetPosition(position *core.GeoPosition) error {
	return nil
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error { return nil }
