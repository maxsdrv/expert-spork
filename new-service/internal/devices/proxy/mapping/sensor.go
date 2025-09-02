package mapping

import (
	"encoding/json"
	"strconv"
	"strings"
	"unicode"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
	"dds-provider/internal/services/parsers"
)

var mappingError = core.ProviderError()

func ConvertToSensorInfoDynamic(data json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	var sensorErr = core.ProviderErrorFn("mapping: to sensor info dynamic")

	sensorData, err := parsers.ParseSensorInfo(data)
	if err != nil {
		return nil, sensorErr("%v", err)
	}

	positionMode, err := convertToPositionMode(sensorData.PositionMode)
	if err != nil {
		return nil, sensorErr("%v", err)
	}

	jammerMode, err := convertToJammerMode(sensorData.JammerMode)
	if err != nil {
		return nil, sensorErr("%v", err)
	}

	sensorInfo := &core.SensorInfoDynamic{
		SensorId:          deviceId,
		Disabled:          sensorData.Disabled,
		State:             string(sensorData.State),
		Position:          convertToPosition(sensorData.Position),
		PositionMode:      positionMode,
		Workzone:          convertToWorkZone(sensorData.Workzone),
		JammerMode:        jammerMode,
		JammerAutoTimeout: sensorData.JammerAutoTimeout,
		HwInfo:            convertToHwInfo(sensorData.HwInfo),
	}

	return sensorInfo, nil
}

func ConvertToSensorInfo(sensorInfo provider_client.SensorInfo) (*core.SensorInfo, error) {
	var sensorType core.SensorType

	switch sensorInfo.Type {
	case provider_client.SENSORTYPE_RFD:
		sensorType = apiv1.SensorType_SENSOR_RFD
	case provider_client.SENSORTYPE_RADAR:
		sensorType = apiv1.SensorType_SENSOR_RADAR
	case provider_client.SENSORTYPE_CAMERA:
		sensorType = apiv1.SensorType_SENSOR_CAMERA
	default:
		return nil, mappingError("to sensor info: unknown SensorType: %v", sensorInfo.Type)
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
	}, nil
}

func convertToHwInfo(dssHwInfo *provider_client.HwInfo) *core.HwInfo {
	if dssHwInfo == nil {
		return nil
	}

	hwInfo := &core.HwInfo{}

	if dssHwInfo.Temperature != nil {
		cleanTemp := strings.TrimFunc(*dssHwInfo.Temperature, func(r rune) bool {
			return !unicode.IsDigit(r) && r != '.' && r != '-'
		})
		if temp, err := strconv.ParseFloat(cleanTemp, 32); err == nil {
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

func convertToJammerMode(jammerMode *provider_client.JammerMode) (*core.JammerMode, error) {
	if jammerMode == nil {
		return nil, nil
	}

	switch *jammerMode {
	case provider_client.JAMMERMODE_AUTO:
		mode := apiv1.JammerMode_JAMMER_AUTO
		return &mode, nil
	case provider_client.JAMMERMODE_MANUAL:
		mode := apiv1.JammerMode_JAMMER_MANUAL
		return &mode, nil
	default:
		return nil, mappingError("unknown JammerMode: %v", *jammerMode)
	}
}
