package mapping

import (
	"context"
	"encoding/json"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

var logging = ctx_log.GetLogger(nil)

func ConvertToSensorInfoDynamic(ctx context.Context, data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
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

	if position := convertToAPIPosition(&sensorData.Position); position != nil {
		if position.Coordinate != nil {
			lat := position.Coordinate.GetLatitude()
			lon := position.Coordinate.GetLongitude()
			azimuth := position.GetAzimuth()
			sensorInfo.Position = core.NewGeoPosition(lat, lon, azimuth)
		}
	}

	if positionMode := convertToAPIPositionMode(&sensorData.PositionMode); positionMode != nil {
		sensorInfo.PositionMode = core.GeoPositionMode(*positionMode)
	}

	if workZone := convertToAPIWorkZone(sensorData.Workzone); workZone != nil {
		if len(workZone) > 0 {
			sector := workZone[0]
			number := int(sector.GetNumber())
			distance := sector.GetDistance()
			minAngle := sector.GetMinAngle()
			maxAngle := sector.GetMaxAngle()
			sensorInfo.Workzone = core.NewDeviceWorkzoneSector(number, distance, minAngle, maxAngle)
		}
	}

	if hwInfo := convertToAPIHwInfo(sensorData.HwInfo); hwInfo != nil {
		sensorInfo.HwInfo = core.NewHwInfo(hwInfo.Temperature, hwInfo.Voltage, nil)
	}

	return sensorInfo, nil
}

func ConvertToAPISensorInfo(sensorInfo dss_target_service.SensorInfo) *apiv1.SensorInfo {
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

func convertToAPIHwInfo(dssHwInfo *dss_target_service.HwInfo) *apiv1.HwInfo {
	hwInfo := &apiv1.HwInfo{
		Temperature: dssHwInfo.Temperature,
		Voltage:     dssHwInfo.Voltage,
	}
	return hwInfo
}
