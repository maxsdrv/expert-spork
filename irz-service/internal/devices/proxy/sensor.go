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
	dssMode := mapping.ConvertJammerMode(mode)

	setJammerModeReq := provider_client.SetJammerModeRequest{
		Id:         s.id,
		JammerMode: dssMode,
		Timeout:    int32(timeout.Seconds()),
	}

	_, err := s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	return err
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	sensorCoreInfo := mapping.ConvertToSensorInfo(*s.info)
	return *sensorCoreInfo.ToAPI()
}

func (s *Sensor) SetDisabled(disabled bool) error {
	return nil
}

func (s *Sensor) SetPosition(position *core.GeoPosition) error {
	return nil
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error { return nil }
