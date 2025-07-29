package proxy

import (
	"context"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/wsclient"
)

type Sensor struct {
	sensorId     string
	serviceAPI   *dss_target_service.SensorAPIService
	sensorMapper *wsclient.SensorDataMapper
	info         *dss_target_service.SensorInfo
}

func NewSensor(sensorId string, api *dss_target_service.SensorAPIService, info *dss_target_service.SensorInfo) *Sensor {
	return &Sensor{
		sensorId:     sensorId,
		serviceAPI:   api,
		sensorMapper: wsclient.NewSensorDataMapper(),
		info:         info,
	}
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout int32) error {
	dssMode, err := s.sensorMapper.ConvertJammerMode(mode)
	if err != nil {
		return err
	}

	setJammerModeReq := dss_target_service.SetJammerModeRequest{
		Id:         s.sensorId,
		JammerMode: dssMode,
		Timeout:    timeout,
	}

	_, err = s.serviceAPI.SetJammerMode(context.Background()).
		SetJammerModeRequest(setJammerModeReq).
		Execute()

	return err
}

func (s *Sensor) SensorInfo() apiv1.SensorInfo {
	return *s.sensorMapper.ConvertToAPISensorInfo(*s.info)
}

func (s *Sensor) GetSensorId() string {
	return s.sensorId
}

func (s *Sensor) SetDisabled(disabled bool) {
	return
}

func (s *Sensor) SetPosition(position *core.GeoPosition) error {
	return nil
}
