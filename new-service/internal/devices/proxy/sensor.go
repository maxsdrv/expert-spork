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
	cameraAPI  *provider_client.CameraAPIService
	info       *provider_client.SensorInfo
}

func NewSensor(sensorId string, api *provider_client.APIClient, info *provider_client.SensorInfo) (*Sensor, error) {
	if sensorId == "" {
		return nil, proxyError("sensor id is empty")
	}
	return &Sensor{
		id:         sensorId,
		serviceAPI: api.SensorAPI,
		deviceAPI:  api.DeviceAPI,
		cameraAPI:  api.CameraAPI,
		info:       info,
	}, nil
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout time.Duration) error {
	dssMode, err := mapping.ConvertToJammerMode(mode)
	if err != nil {
		return err
	}

	setJammerModeReq := provider_client.SetJammerModeRequest{
		Id:         s.id,
		JammerMode: dssMode,
		Timeout:    int32(timeout.Seconds()),
	}

	_, err = s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	if err != nil {
		return proxyError("SetJammerMode: %s, %v", s.id, err)
	}

	return nil
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

	if err != nil {
		return proxyError("SetDisabled: %s, %v", s.id, err)
	}

	return nil
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

	if err != nil {
		return proxyError("SetPosition: %s, %v", s.id, err)
	}
	return nil
}

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error {
	dssMode, err := mapping.ConvertToProviderGeoPositionMode(mode)
	if err != nil {
		return err
	}

	setPositionModeReq := provider_client.SetPositionModeRequest{
		Id:           s.id,
		PositionMode: dssMode,
	}

	_, err = s.deviceAPI.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	if err != nil {
		return proxyError("SetPositionMode: %s, %v", s.id, err)
	}

	return nil
}

func (s *Sensor) GetCamera() (string, error) {
	resp, _, err := s.cameraAPI.GetId(context.Background()).Id(s.id).Execute()
	if err != nil {
		return "", proxyError("GetCameraId: %s, %v", s.id, err)
	}
	if resp == nil || resp.CameraId == "" {
		return "", proxyError("GetCameraId: camera id not found for sensor %s", s.id)
	}
	return resp.CameraId, nil
}
