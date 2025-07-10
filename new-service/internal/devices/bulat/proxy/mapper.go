package proxy

import (
	"context"
	"encoding/json"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

type SensorDataMapper struct{}

func NewSensorDataMapper() *SensorDataMapper {
	return &SensorDataMapper{}
}

func (m *SensorDataMapper) ConvertToSensorInfoDynamic(ctx context.Context, data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	var sensorData dss_target_service.SensorInfo
	if err := json.Unmarshal(data, &sensorData); err != nil {
		logger.WithError(err).Error("Convert to SensorInfoDynamicResponse")
		return nil, err
	}

	sensorInfo := &core.SensorInfoDynamic{
		DeviceId: deviceId,
		Disabled: &sensorData.Disabled,
		State:    (*string)(&sensorData.State),
	}

	if position := m.convertPosition(&sensorData.Position); position != nil {
		sensorInfo.Position = position
	}

	if positionMode := m.convertPositionMode(&sensorData.PositionMode); positionMode != nil {
		sensorInfo.PositionMode = positionMode
	}

	if workZone := m.convertWorkZone(sensorData.Workzone); workZone != nil {
		sensorInfo.Workzone = workZone
	}

	if hwInfo := m.convertHwInfo(sensorData.HwInfo); hwInfo != nil {
		sensorInfo.HwInfo = hwInfo
	}

	return sensorInfo, nil
}

func (m *SensorDataMapper) ConvertJammerMode(jammerMode apiv1.JammerMode) (dss_target_service.JammerMode, error) {
	switch jammerMode {
	case apiv1.JammerMode_JAMMER_MODE_AUTO:
		return dss_target_service.JAMMERMODE_AUTO, nil
	case apiv1.JammerMode_JAMMER_MODE_MANUAL:
		return dss_target_service.JAMMERMODE_MANUAL, nil
	default:
		return dss_target_service.JAMMERMODE_UNDEFINED, nil
	}
}

func (m *SensorDataMapper) convertPosition(pos *dss_target_service.GeoPosition) *apiv1.GeoPosition {
	if pos == nil {
		return nil
	}

	coordinate := &apiv1.GeoCoordinate{
		Latitude:  &pos.Coordinate.Latitude,
		Longitude: &pos.Coordinate.Longitude,
		Altitude:  pos.Coordinate.Altitude,
	}

	return &apiv1.GeoPosition{
		Azimuth:    &pos.Azimuth,
		Coordinate: coordinate,
	}
}

func (m *SensorDataMapper) convertPositionMode(posMode *dss_target_service.GeoPositionMode) *apiv1.GeoPositionMode {

	switch *posMode {
	case dss_target_service.GEOPOSITIONMODE_AUTO:
		mode := apiv1.GeoPositionMode_GEO_POSITION_MODE_AUTO
		return &mode
	case dss_target_service.GEOPOSITIONMODE_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_POSITION_MODE_MANUAL
		return &mode
	case dss_target_service.GEOPOSITIONMODE_ALWAYS_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_POSITION_MODE_ALWAYS_MANUAL
		return &mode
	default:
		return nil
	}
}

func (m *SensorDataMapper) convertWorkZone(dssWorkZone []dss_target_service.WorkzoneSector) []*apiv1.WorkzoneSector {
	if len(dssWorkZone) == 0 {
		return nil
	}

	workZones := make([]*apiv1.WorkzoneSector, 0, len(dssWorkZone))
	for _, sector := range dssWorkZone {
		workZoneSector := &apiv1.WorkzoneSector{
			Number:   &sector.Number,
			Distance: &sector.Distance,
			MinAngle: &sector.MinAngle,
			MaxAngle: &sector.MaxAngle,
		}
		workZones = append(workZones, workZoneSector)
	}
	return workZones
}

func (m *SensorDataMapper) convertHwInfo(dssHwInfo *dss_target_service.HwInfo) *apiv1.HwInfo {
	hwInfo := &apiv1.HwInfo{
		Temperature: dssHwInfo.Temperature,
		Voltage:     dssHwInfo.Voltage,
	}
	return hwInfo
}
