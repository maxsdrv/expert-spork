package core

import (
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	"dds-provider/internal/generated/api/proto"
)

func TestJammerInfo(id DeviceId) *apiv1.JammerInfo {
	sensorId := NewId("9f86d081-884c-4d63-a159-f35e7e5b72a0")
	deviceIdStr := id.String()
	sensorIdStr := sensorId.String()
	return &apiv1.JammerInfo{
		JammerId:  &deviceIdStr,
		Model:     proto.String("JM-4000"),
		Serial:    proto.String("SN12345678"),
		SwVersion: proto.String("1.2.3"),
		SensorId:  &sensorIdStr,
		GroupId:   proto.String("group1"),
	}
}

type JammerInfoDynamic struct {
	DeviceId      DeviceId
	Disabled      bool
	State         string
	Position      GeoPosition
	PositionMode  GeoPositionMode
	Workzone      DeviceWorkzone
	Bands         JammerBands
	BandOptions   BandOptions
	HwInfo        HwInfo
	TimeoutStatus JammerTimeoutStatus
}

func (j *JammerInfoDynamic) Id() DeviceId {
	return j.DeviceId
}

func (j *JammerInfoDynamic) ToAPI() *apiv1.JammerInfoDynamicResponse {
	apiPosition := (*apiv1.GeoPosition)(&j.Position)
	apiPositionMode := (*apiv1.GeoPositionMode)(&j.PositionMode)
	apiHwInfo := (*apiv1.HwInfo)(&j.HwInfo)
	apiJammerTimeoutStatus := (*apiv1.JammerTimeoutStatus)(&j.TimeoutStatus)

	var apiWorkzone []*apiv1.WorkzoneSector
	for i := range j.Workzone {
		apiWorkzone = append(apiWorkzone, (*apiv1.WorkzoneSector)(&j.Workzone[i]))
	}

	var apiBands []*apiv1.Band
	activeBands := j.Bands.GetActive()
	for _, band := range j.Bands.GetAll() {
		isActive := false
		for _, activeBand := range activeBands {
			if band == activeBand {
				isActive = true
				break
			}
		}
		apiBands = append(apiBands, &apiv1.Band{
			Label:  &band,
			Active: proto.Bool(isActive),
		})
	}

	var apiBandOptions []*apiv1.BandOption
	for i := range j.BandOptions {
		apiBandOptions = append(apiBandOptions, (*apiv1.BandOption)(&j.BandOptions[i]))
	}

	return &apiv1.JammerInfoDynamicResponse{
		Disabled:      &j.Disabled,
		State:         &j.State,
		Position:      apiPosition,
		PositionMode:  apiPositionMode,
		Workzone:      apiWorkzone,
		Bands:         apiBands,
		BandOptions:   apiBandOptions,
		HwInfo:        apiHwInfo,
		TimeoutStatus: apiJammerTimeoutStatus,
	}
}

func TestJammerInfoDynamic(id DeviceId) *JammerInfoDynamic {
	return &JammerInfoDynamic{
		DeviceId:     id,
		Disabled:     false,
		State:        "ACTIVE",
		PositionMode: GeoPositionMode(apiv1.GeoPositionMode_GEO_MANUAL),
		Position: GeoPosition(apiv1.GeoPosition{
			Azimuth: proto.Float64(45.5),
			Coordinate: &apiv1.GeoCoordinate{
				Latitude:  proto.Float64(55.7558),
				Longitude: proto.Float64(37.6173),
				Altitude:  proto.Float64(120.5),
			},
		}),
		Workzone: DeviceWorkzone{
			DeviceWorkzoneSector(apiv1.WorkzoneSector{
				Number:   proto.Int32(1),
				Distance: proto.Float64(100.0),
				MinAngle: proto.Float64(0.0),
				MaxAngle: proto.Float64(90.0),
			}),
			DeviceWorkzoneSector(apiv1.WorkzoneSector{
				Number:   proto.Int32(2),
				Distance: proto.Float64(150.0),
				MinAngle: proto.Float64(90.0),
				MaxAngle: proto.Float64(180.0),
			}),
		},
		Bands: func() JammerBands {
			bands, _ := NewBands(BandList{"900MHz", "1800MHz", "2100MHz"}, BandList{"900MHz", "2100MHz"})
			return bands
		}(),
		BandOptions: NewBandOptions([][]string{
			{"900MHz", "1800MHz", "2100MHz"},
		}),
		HwInfo: HwInfo(apiv1.HwInfo{
			Temperature: proto.String("36.5"),
			Voltage:     proto.String("12.0"),
			Current:     proto.String("2.5"),
		}),
		TimeoutStatus: JammerTimeoutStatus(apiv1.JammerTimeoutStatus{
			Started:  timestamppb.Now(),
			Now:      timestamppb.Now(),
			Duration: proto.Int32(300),
		}),
	}
}
