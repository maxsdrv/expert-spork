package proxy

import (
	"context"
	"time"

	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy/mapping"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

type Sensor struct {
	id         string
	serviceAPI *provider_client.SensorAPIService
	deviceAPI  *provider_client.DeviceAPIService
	info       *provider_client.SensorInfo
}

func NewSensor(sensorId string, api *provider_client.APIClient, info *provider_client.SensorInfo) *Sensor {
	return &Sensor{
		id:         sensorId,
		serviceAPI: api.SensorAPI,
		deviceAPI:  api.DeviceAPI,
		info:       info,
	}
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout time.Duration) error {
	dssMode := mapping.ConvertToJammerMode(mode)

	setJammerModeReq := provider_client.SetJammerModeRequest{
		Id:         s.id,
		JammerMode: dssMode,
		Timeout:    int32(timeout.Seconds()),
	}

	_, err := s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	return handleSensorError("SetJammerMode", s.id, err)
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	sensorCoreInfo, _ := mapping.ConvertToSensorInfo(*s.info)

	return *sensorCoreInfo.ToAPI()
}

func (s *Sensor) SetDisabled(disabled bool) error {
	setDisabledReq := provider_client.SetDisabledRequest{
		Id:       s.id,
		Disabled: disabled,
	}

	_, err := s.deviceAPI.SetDisabled(context.Background()).
		SetDisabledRequest(setDisabledReq).
		Execute()

	return handleSensorError("SetDisabled", s.id, err)
}

func (s *Sensor) SetPosition(position core.GeoPosition) error {
	dssPosition := mapping.ConvertToProviderGeoPosition(position)
	setPositionReq := provider_client.SetPositionRequest{
		Id:       s.id,
		Position: dssPosition,
	}

	_, err := s.deviceAPI.SetPosition(context.Background()).
		SetPositionRequest(setPositionReq).
		Execute()

	return handleSensorError("SetPosition", s.id, err)
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error {
	dssMode := mapping.ConvertToProviderGeoPositionMode(mode)

	setPositionModeReq := provider_client.SetPositionModeRequest{
		Id:           s.id,
		PositionMode: dssMode,
	}

	_, err := s.deviceAPI.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	return handleSensorError("SetPositionMode", s.id, err)
}
