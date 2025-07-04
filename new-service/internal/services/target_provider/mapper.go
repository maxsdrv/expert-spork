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

	var sensorData core.SensorInfoDynamic
	if err := json.Unmarshal(data, &sensorData); err != nil {
		logger.WithError(err).Error("Convert to SensorInfoDynamicResponse")
		return nil, err
	}

	sensorInfo := &core.SensorInfoDynamic{
		DeviceId: deviceId,
		Disabled: sensorData.Disabled,
		State:    sensorData.State,
	}

	if position := m.convertPosition(sensorData.Position); position != nil {
		sensorInfo.Position = position
	}

	return sensorInfo, nil
}

func (m *SensorDataMapper) convertPosition(pos *apiv1.GeoPosition) *apiv1.GeoPosition {
	coordinate := &apiv1.GeoCoordinate{
		Latitude:  pos.Coordinate.Latitude,
		Longitude: pos.Coordinate.Longitude,
		Altitude:  pos.Coordinate.Altitude,
	}

	return &apiv1.GeoPosition{
		Azimuth:    pos.Azimuth,
		Coordinate: coordinate,
	}
}
