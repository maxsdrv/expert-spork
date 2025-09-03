package mapping

import (
	"encoding/json"
	"time"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

func ConvertToJammerInfoDynamic(dataRaw json.RawMessage, deviceId core.DeviceId) (*core.JammerInfoDynamic, error) {
	var jammerErr = core.ProviderErrorFn("mapping: to jammer info dynamic")

	var jammerData provider_client.JammerInfo
	if err := json.Unmarshal(dataRaw, &jammerData); err != nil {
		return nil, jammerErr("%v", err)
	}

	positionMode, err := convertToPositionMode(jammerData.PositionMode)
	if err != nil {
		return nil, jammerErr("%v", err)
	}

	jammerInfo := &core.JammerInfoDynamic{
		JammerId:     deviceId,
		Disabled:     jammerData.Disabled,
		State:        string(jammerData.State),
		Position:     convertToPosition(jammerData.Position),
		PositionMode: positionMode,
		Workzone:     convertToWorkZone(jammerData.Workzone),
		HwInfo:       convertToHwInfo(jammerData.HwInfo),
	}

	if jammerData.Bands != nil {
		var allBands core.BandList
		var activeBands core.BandList

		for _, band := range jammerData.Bands {
			if band.Label != "" {
				allBands = append(allBands, band.Label)
				if band.Active {
					activeBands = append(activeBands, band.Label)
				}
			}
		}

		if bands, err := core.NewBands(allBands, activeBands); err == nil {
			jammerInfo.Bands = bands
		} else {
			return nil, jammerErr("%v", err)
		}

		if bandOptions, ok := jammerData.GetBandOptionsOk(); ok && bandOptions != nil {
			options := core.NewBandOptions(bandOptions)
			jammerInfo.BandOptions = &options
		} else {
			jammerInfo.BandOptions = nil
		}
	}

	if jammerData.TimeoutStatus != nil {
		jammerInfo.TimeoutStatus = &core.JammerTimeoutStatus{
			Started:  jammerData.TimeoutStatus.Started,
			Now:      jammerData.TimeoutStatus.Now,
			Duration: time.Duration(jammerData.TimeoutStatus.Duration),
		}
	}

	return jammerInfo, nil
}

func ConvertToJammerInfo(jammerInfo provider_client.JammerInfo) *core.JammerInfo {
	var sensorId *core.DeviceId
	if jammerInfo.SensorId != nil {
		id := core.NewId(*jammerInfo.SensorId)
		sensorId = &id
	}

	return &core.JammerInfo{
		JammerId:  core.NewId(jammerInfo.Id),
		Model:     jammerInfo.Model,
		Serial:    jammerInfo.Serial,
		SwVersion: jammerInfo.SwVersion,
		SensorId:  sensorId,
		GroupId:   jammerInfo.GroupId,
	}
}

func ConvertToJammerMode(jammerMode core.JammerMode) (provider_client.JammerMode, error) {
	switch jammerMode {
	case apiv1.JammerMode_JAMMER_AUTO:
		return provider_client.JAMMERMODE_AUTO, nil
	case apiv1.JammerMode_JAMMER_MANUAL:
		return provider_client.JAMMERMODE_MANUAL, nil
	default:
		return provider_client.JAMMERMODE_UNKNOWN_DEFAULT_OPEN_API, mappingError("to jammer mode: unknown jammer mode %v", jammerMode)
	}
}
