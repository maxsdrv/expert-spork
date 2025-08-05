package proxy

import (
	"context"
	"errors"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/mapping"
)

type Sensor struct {
	id         string
	serviceAPI *dss_target_service.SensorAPIService
	info       *dss_target_service.SensorInfo
}

func NewSensor(sensorId string, api *dss_target_service.SensorAPIService, info *dss_target_service.SensorInfo) (*Sensor, error) {
	if sensorId == "" {
		return nil, errors.New("sensor id required")
	}
	return &Sensor{
		id:         sensorId,
		serviceAPI: api,
		info:       info,
	}, nil
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout int32) error {
	dssMode := mapping.ConvertToJammerMode(mode)

	setJammerModeReq := dss_target_service.SetJammerModeRequest{
		Id:         s.id,
		JammerMode: dssMode,
		Timeout:    timeout,
	}

	_, err := s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	if err != nil {
		return handleSensorError("SetJammerMode", s.id, err)
	}

	return nil
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	return *mapping.ConvertToSensorInfo(*s.info)
}

func (s *Sensor) SetDisabled(disabled bool) {
	return
}

func (s *Sensor) SetPosition(position *core.GeoPosition) error {
	return nil
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error { return nil }
