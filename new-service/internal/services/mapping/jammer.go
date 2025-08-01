package mapping

import (
	"context"
	"encoding/json"

	"google.golang.org/protobuf/types/known/timestamppb"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func ConvertToJammerInfoDynamic(ctx context.Context, dataRaw json.RawMessage, deviceId core.DeviceId) (*core.JammerInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	var jammerData dss_target_service.JammerInfo
	if err := json.Unmarshal(dataRaw, &jammerData); err != nil {
		logger.WithError(err).Error("Convert to JammerInfoDynamicResponse")
		return nil, err
	}

	jammerInfo := &core.JammerInfoDynamic{
		DeviceId: deviceId,
		Disabled: jammerData.Disabled,
		State:    string(jammerData.State),
	}

	if position := convertToAPIPosition(&jammerData.Position); position != nil {
		if position.Coordinate != nil {
			lat := position.Coordinate.GetLatitude()
			lon := position.Coordinate.GetLongitude()
			azimuth := position.GetAzimuth()
			jammerInfo.Position = core.NewGeoPosition(lat, lon, azimuth)
		}
	}

	if positionMode := convertToAPIPositionMode(&jammerData.PositionMode); positionMode != nil {
		jammerInfo.PositionMode = core.GeoPositionMode(*positionMode)
	}
	if workZone := convertToAPIWorkZone(jammerData.Workzone); workZone != nil {
		if len(workZone) > 0 {
			sector := workZone[0]
			number := int(sector.GetNumber())
			distance := sector.GetDistance()
			minAngle := sector.GetMinAngle()
			maxAngle := sector.GetMaxAngle()

			deviceWorkzone := core.DeviceWorkzone{
				core.NewDeviceWorkzoneSector(number, distance, minAngle, maxAngle),
			}
			jammerInfo.Workzone = deviceWorkzone
		}
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
			logger.WithError(err).Error("Failed to create bands")
			return nil, err
		}

		if jammerData.BandOptions != nil {
			jammerInfo.BandOptions = core.NewBandOptions(jammerData.BandOptions)
		}
	}

	if hwInfo := convertToAPIHwInfo(jammerData.HwInfo); hwInfo != nil {
		jammerInfo.HwInfo = core.NewHwInfo(hwInfo.Temperature, hwInfo.Voltage, nil)
	}

	if jammerData.TimeoutStatus != nil {
		jammerInfo.TimeoutStatus = core.NewJammerTimeoutStatus(
			timestamppb.New(jammerData.TimeoutStatus.Started),
			timestamppb.New(jammerData.TimeoutStatus.Now),
			jammerData.TimeoutStatus.Duration,
		)
	}

	return jammerInfo, nil
}

func ConvertToAPIJammerInfo(jammerInfo dss_target_service.JammerInfo) *apiv1.JammerInfo {
	return &apiv1.JammerInfo{
		JammerId:  &jammerInfo.Id,
		Model:     &jammerInfo.Model,
		Serial:    jammerInfo.Serial,
		SwVersion: jammerInfo.SwVersion,
		SensorId:  jammerInfo.SensorId,
		GroupId:   jammerInfo.GroupId,
	}
}

func ConvertJammerMode(jammerMode core.JammerMode) (dss_target_service.JammerMode, error) {
	switch jammerMode {
	case core.JammerMode(apiv1.JammerMode_JAMMER_AUTO):
		return dss_target_service.JAMMERMODE_AUTO, nil
	case core.JammerMode(apiv1.JammerMode_JAMMER_MANUAL):
		return dss_target_service.JAMMERMODE_MANUAL, nil
	default:
		return dss_target_service.JAMMERMODE_UNDEFINED, nil
	}
}
