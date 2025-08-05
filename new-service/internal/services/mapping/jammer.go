package mapping

import (
	"encoding/json"
	"time"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func ConvertToJammerInfoDynamic(dataRaw json.RawMessage, deviceId core.DeviceId) (*core.JammerInfoDynamic, error) {
	var jammerData dss_target_service.JammerInfo
	if err := json.Unmarshal(dataRaw, &jammerData); err != nil {
		return nil, mappingError("to jammer info dynamic: %v", err)
	}

	jammerInfo := &core.JammerInfoDynamic{
		JammerId:     deviceId,
		Disabled:     jammerData.Disabled,
		State:        string(jammerData.State),
		Position:     convertToPosition(jammerData.Position),
		PositionMode: convertToPositionMode(jammerData.PositionMode),
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
			return nil, mappingError("to jammer info dynamic: %v", err)
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

func ConvertToJammerInfo(jammerInfo dss_target_service.JammerInfo) *core.JammerInfo {
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

func ConvertToJammerMode(jammerMode core.JammerMode) dss_target_service.JammerMode {
	switch jammerMode {
	case apiv1.JammerMode_JAMMER_AUTO:
		return dss_target_service.JAMMERMODE_AUTO
	case apiv1.JammerMode_JAMMER_MANUAL:
		return dss_target_service.JAMMERMODE_MANUAL
	default:
		return dss_target_service.JAMMERMODE_UNDEFINED
	}
}
