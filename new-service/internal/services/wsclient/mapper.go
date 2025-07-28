package wsclient

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
		Disabled: sensorData.Disabled,
		State:    string(sensorData.State),
	}

	if position := m.convertToAPIPosition(&sensorData.Position); position != nil {
		if position.Coordinate != nil {
			lat := position.Coordinate.GetLatitude()
			lon := position.Coordinate.GetLongitude()
			azimuth := position.GetAzimuth()
			sensorInfo.Position = core.NewGeoPosition(lat, lon, azimuth)
		}
	}

	if positionMode := m.convertToAPIPositionMode(&sensorData.PositionMode); positionMode != nil {
		sensorInfo.PositionMode = core.GeoPositionMode(*positionMode)
	}

	if workZone := m.convertToAPIWorkZone(sensorData.Workzone); workZone != nil {
		if len(workZone) > 0 {
			sector := workZone[0]
			number := int(sector.GetNumber())
			distance := sector.GetDistance()
			minAngle := sector.GetMinAngle()
			maxAngle := sector.GetMaxAngle()
			sensorInfo.Workzone = core.NewDeviceWorkzoneSector(number, distance, minAngle, maxAngle)
		}
	}

	if hwInfo := m.convertToAPIHwInfo(sensorData.HwInfo); hwInfo != nil {
		sensorInfo.HwInfo = core.NewHwInfo(hwInfo.Temperature, hwInfo.Voltage, nil)
	}

	return sensorInfo, nil
}

func (m *SensorDataMapper) ConvertJammerMode(jammerMode core.JammerMode) (dss_target_service.JammerMode, error) {
	switch jammerMode {
	case core.JammerMode(apiv1.JammerMode_JAMMER_AUTO):
		return dss_target_service.JAMMERMODE_AUTO, nil
	case core.JammerMode(apiv1.JammerMode_JAMMER_MANUAL):
		return dss_target_service.JAMMERMODE_MANUAL, nil
	default:
		return dss_target_service.JAMMERMODE_UNDEFINED, nil
	}
}

func (m *SensorDataMapper) ConvertToAPISensorInfo(sensorInfo dss_target_service.SensorInfo) *apiv1.SensorInfo {
	var sensorType *apiv1.SensorType

	switch sensorInfo.Type {
	case dss_target_service.SENSORTYPE_RFD:
		t := apiv1.SensorType_SENSOR_RFD
		sensorType = &t
	case dss_target_service.SENSORTYPE_RADAR:
		t := apiv1.SensorType_SENSOR_RADAR
		sensorType = &t
	case dss_target_service.SENSORTYPE_CAMERA:
		t := apiv1.SensorType_SENSOR_CAMERA
		sensorType = &t
	default:
		sensorType = nil
	}

	var jammerIds []string
	if sensorInfo.JammerIds != nil {
		jammerIds = sensorInfo.JammerIds
	}

	return &apiv1.SensorInfo{
		SensorId:  &sensorInfo.Id,
		Type:      sensorType,
		Model:     &sensorInfo.Model,
		Serial:    sensorInfo.Serial,
		SwVersion: sensorInfo.SwVersion,
		JammerIds: jammerIds,
	}
}

func (m *SensorDataMapper) convertToAPIPosition(pos *dss_target_service.GeoPosition) *apiv1.GeoPosition {
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

func (m *SensorDataMapper) convertToAPIPositionMode(posMode *dss_target_service.GeoPositionMode) *apiv1.GeoPositionMode {

	switch *posMode {
	case dss_target_service.GEOPOSITIONMODE_AUTO:
		mode := apiv1.GeoPositionMode_GEO_AUTO
		return &mode
	case dss_target_service.GEOPOSITIONMODE_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_MANUAL
		return &mode
	case dss_target_service.GEOPOSITIONMODE_ALWAYS_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_ALWAYS_MANUAL
		return &mode
	default:
		return nil
	}
}

func (m *SensorDataMapper) convertToAPIWorkZone(dssWorkZone []dss_target_service.WorkzoneSector) []*apiv1.WorkzoneSector {
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

func (m *SensorDataMapper) convertToAPIHwInfo(dssHwInfo *dss_target_service.HwInfo) *apiv1.HwInfo {
	hwInfo := &apiv1.HwInfo{
		Temperature: dssHwInfo.Temperature,
		Voltage:     dssHwInfo.Voltage,
	}
	return hwInfo
}
