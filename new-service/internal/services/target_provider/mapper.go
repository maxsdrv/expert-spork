package target_provider

import (
	"context"
	"dds-provider/internal/core"
	"encoding/json"

	apiv1 "dds-provider/internal/generated/api/proto"
)

type SensorDataMapper struct{}

func NewSensorDataMapper() *SensorDataMapper {
	return &SensorDataMapper{}
}

func (m *SensorDataMapper) ConvertToSensorInfoDynamic(ctx context.Context, data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("SensorInfoDynamicResponse")

	var sensorData apiv1.SensorInfoDynamicResponse
	if err := json.Unmarshal(data, &sensorData); err != nil {
		logger.WithError(err).Error("SensorInfoDynamicResponse")
		return nil, err
	}

	sensorInfo := &core.SensorInfoDynamic{
		DeviceId: deviceId,
	}

	if position := m.convertPosition(); position != nil {
		sensorInfo.Position = position
	}

	return sensorInfo, nil
}

func (m *SensorDataMapper) convertPosition() *apiv1.GeoPosition {
	//coordinate := &apiv1.GeoCoordinate{}

	return &apiv1.GeoPosition{}
}
