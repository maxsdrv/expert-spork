package proxy

import (
	"context"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/mapping"
)

type Sensor struct {
	id         string
	serviceAPI *dss_target_service.SensorAPIService
	deviceAPI  *dss_target_service.DeviceAPIService
	info       *dss_target_service.SensorInfo
}

func NewSensor(sensorId string, api *dss_target_service.APIClient, info *dss_target_service.SensorInfo) (*Sensor, error) {
	if sensorId == "" {
		return nil, proxyError("sensor id required")
	}
	return &Sensor{
		id:         sensorId,
		serviceAPI: api.SensorAPI,
		deviceAPI:  api.DeviceAPI,
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

	return handleSensorError("SetJammerMode", s.id, err)
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	sensorCoreInfo := mapping.ConvertToSensorInfo(*s.info)
	return *sensorCoreInfo.ToAPI()
}

func (s *Sensor) SetDisabled(disabled bool) {
	setDisabledReq := dss_target_service.SetDisabledRequest{
		Id:       s.id,
		Disabled: disabled,
	}

	_, _ = s.deviceAPI.SetDisabled(context.Background()).
		SetDisabledRequest(setDisabledReq).
		Execute()
}

func (s *Sensor) SetPosition(position *core.GeoPosition) error {
	if position == nil {
		return proxyError("position is required")
	}

	dssPosition := mapping.ConvertToDSSGeoPosition(*position)
	setPositionReq := dss_target_service.SetPositionRequest{
		Id:       s.id,
		Position: dssPosition,
	}

	_, err := s.deviceAPI.SetPosition(context.Background()).
		SetPositionRequest(setPositionReq).
		Execute()

	return handleSensorError("SetPosition", s.id, err)
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error {
	dssMode := mapping.ConvertToDSSGeoPositionMode(mode)

	setPositionModeReq := dss_target_service.SetPositionModeRequest{
		Id:           s.id,
		PositionMode: dssMode,
	}

	_, err := s.deviceAPI.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	return handleSensorError("SetPositionMode", s.id, err)
}
