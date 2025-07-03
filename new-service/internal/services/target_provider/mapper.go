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

type SensorData struct {
	ID                string
	Type              string
	Model             string
	Serial            string
	SwVersion         string
	Disabled          bool
	State             string
	Position          Position
	PositionMode      string
	Workzone          []Workzone
	JammerIds         []string
	JammerMode        string
	JammerAutoTimeout int32
	HwInfo            *HwInfo
}

type Position struct {
	azimuth    float64
	coordinate Coordinate
}

type Coordinate struct {
	latitude  float64
	longitude float64
	altitude  float64
}

type Workzone struct {
	number   int32
	distance float64
	minAngle float64
	maxAngle float64
}

type HwInfo struct {
	temperature string
	voltage     string
	current     string
}

func (m *SensorDataMapper) ConvertToSensorInfoDynamic(ctx context.Context, data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("SensorInfoDynamicResponse")

	var sensorData SensorData
	if err := json.Unmarshal(data, &sensorData); err != nil {
		logger.WithError(err).Error("SensorInfoDynamicResponse")
		return nil, err
	}

	sensorInfo := &core.SensorInfoDynamic{
		DeviceId: deviceId,
	}

	sensorInfo.Disabled = &sensorData.Disabled
	sensorInfo.State = &sensorData.State

	if position := m.convertPosition(sensorData.Position); position != nil {
		sensorInfo.Position = position
	}

	return sensorInfo, nil
}

func (m *SensorDataMapper) convertPosition(pos Position) *apiv1.GeoPosition {
	coordinate := &apiv1.GeoCoordinate{
		Latitude:  &pos.coordinate.latitude,
		Longitude: &pos.coordinate.longitude,
		Altitude:  &pos.coordinate.altitude,
	}

	return &apiv1.GeoPosition{
		Azimuth:    &pos.azimuth,
		Coordinate: coordinate,
	}
}
