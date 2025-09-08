package mapping

import (
	"encoding/json"
	"strconv"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

var mappingError = core.ProviderErrorFn("mapping")

func ConvertToSensorInfoDynamic(data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	var sensorData provider_client.SensorInfo
	if err := json.Unmarshal(data, &sensorData); err != nil {
		return nil, mappingError("to sensor info dynamic: %v", err)
	}

	sensorInfo := &core.SensorInfoDynamic{
		SensorId:          deviceId,
		Disabled:          sensorData.Disabled,
		State:             string(sensorData.State),
		Position:          convertToPosition(sensorData.Position),
		PositionMode:      convertToPositionMode(sensorData.PositionMode),
		Workzone:          convertToWorkZone(sensorData.Workzone),
		JammerMode:        convertToJammerMode(sensorData.JammerMode),
		JammerAutoTimeout: sensorData.JammerAutoTimeout,
		HwInfo:            convertToHwInfo(sensorData.HwInfo),
	}

	return sensorInfo, nil
}

func ConvertToSensorInfo(sensorInfo provider_client.SensorInfo) *core.SensorInfo {
	var sensorType core.SensorType

	switch sensorInfo.Type {
	case provider_client.SENSORTYPE_RFD:
		sensorType = apiv1.SensorType_SENSOR_RFD
	case provider_client.SENSORTYPE_RADAR:
		sensorType = apiv1.SensorType_SENSOR_RADAR
	case provider_client.SENSORTYPE_CAMERA:
		sensorType = apiv1.SensorType_SENSOR_CAMERA
	default:
		sensorType = apiv1.SensorType_SENSOR_RFD
	}

	var jammerIds *[]core.DeviceId
	if sensorInfo.JammerIds != nil {
		var ids []core.DeviceId
		for _, id := range sensorInfo.JammerIds {
			ids = append(ids, core.NewId(id))
		}
		jammerIds = &ids
	}

	return &core.SensorInfo{
		SensorId:   core.NewId(sensorInfo.Id),
		SensorType: sensorType,
		Model:      sensorInfo.Model,
		Serial:     sensorInfo.Serial,
		SwVersion:  sensorInfo.SwVersion,
		JammerIds:  jammerIds,
	}
}

func convertToHwInfo(dssHwInfo *provider_client.HwInfo) *core.HwInfo {
	if dssHwInfo == nil {
		return nil
	}

	hwInfo := &core.HwInfo{}

	if dssHwInfo.Temperature != nil {
		if temp, err := strconv.ParseFloat(*dssHwInfo.Temperature, 32); err == nil {
			tempFloat := float32(temp)
			hwInfo.Temperature = &tempFloat
		}
	}

	if dssHwInfo.Voltage != nil {
		if voltage, err := strconv.ParseFloat(*dssHwInfo.Voltage, 32); err == nil {
			voltageFloat := float32(voltage)
			hwInfo.Voltage = &voltageFloat
		}
	}

	return hwInfo
}

func convertToJammerMode(jammerMode *provider_client.JammerMode) *core.JammerMode {
	if jammerMode == nil {
		return nil
	}

	switch *jammerMode {
	case provider_client.JAMMERMODE_AUTO:
		mode := apiv1.JammerMode_JAMMER_AUTO
		return &mode
	case provider_client.JAMMERMODE_MANUAL:
		mode := apiv1.JammerMode_JAMMER_MANUAL
		return &mode
	default:
		mode := apiv1.JammerMode_JAMMER_AUTO
		return &mode
	}
}
