package proxy

import (
	"context"
	"dds-provider/internal/services/wsclient"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

type Sensor struct {
	sensorId     string
	serviceAPI   *dss_target_service.SensorAPIService
	sensorMapper *wsclient.SensorDataMapper
}

func NewSensor(sensorId string, api *dss_target_service.SensorAPIService) *Sensor {
	return &Sensor{
		sensorId:     sensorId,
		serviceAPI:   api,
		sensorMapper: wsclient.NewSensorDataMapper(),
	}
}

func (s *Sensor) SetJammerMode(mode core.JammerMode, timeout int32) error {
	dssMode, err := s.sensorMapper.convertJammerMode(mode)
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

func (s *Sensor) GetSensorInfo(ctx context.Context) (*apiv1.SensorInfo, error) {
	logger := logging.WithCtxFields(ctx)

	sensorInfo, resp, err := s.serviceAPI.GetSensorInfo(ctx).Id(s.sensorId).Execute()
	if err != nil {
		logger.WithError(err).Error("Get sensor info failed")
		return nil, err
	}
	defer resp.Body.Close()

	info := sensorInfo.GetSensorInfo()

	return s.sensorMapper.convertToAPISensorInfo(info), nil
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

func (s *Sensor) SetPositionMode(mode core.GeoPositionMode) error {
	return nil
}
