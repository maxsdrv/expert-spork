package core

import (
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	apiv1 "dds-provider/internal/generated/api/proto"
)

type GeoPositionMode = apiv1.GeoPositionMode

/*func TestJammerInfo(id DeviceId) *apiv1.JammerInfo {
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
}*/

type JammerInfo struct {
	JammerId  DeviceId
	Model     string
	Serial    *string
	SwVersion *string
	SensorId  *DeviceId
	GroupId   *string
}

type JammerInfoDynamic struct {
	JammerId      DeviceId
	Disabled      bool
	State         string
	Position      GeoPosition
	PositionMode  GeoPositionMode
	Workzone      Workzone
	Bands         JammerBands
	BandOptions   *BandOptions
	HwInfo        *HwInfo
	TimeoutStatus *JammerTimeoutStatus
}

func (j *JammerInfoDynamic) Id() DeviceId {
	return j.JammerId
}

func (j *JammerInfoDynamic) ToAPI() *apiv1.JammerInfoDynamicResponse {
	var apiHwInfo *apiv1.HwInfo
	if j.HwInfo != nil {
		apiHwInfo = j.HwInfo.ToAPI()
	}

	var apiJammerTimeoutStatus *apiv1.JammerTimeoutStatus
	if j.TimeoutStatus != nil {
		apiJammerTimeoutStatus = j.TimeoutStatus.ToAPI()
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
	if j.BandOptions != nil {
		for i := range *j.BandOptions {
			apiBandOptions = append(apiBandOptions, &(*j.BandOptions)[i])
		}
	}

	return &apiv1.JammerInfoDynamicResponse{
		Disabled:      &j.Disabled,
		State:         &j.State,
		Position:      j.Position.ToAPI(),
		PositionMode:  &j.PositionMode,
		Workzone:      j.Workzone.ToAPI(),
		Bands:         apiBands,
		BandOptions:   apiBandOptions,
		HwInfo:        apiHwInfo,
		TimeoutStatus: apiJammerTimeoutStatus,
	}
}

func TestJammerInfoDynamic(id DeviceId) *JammerInfoDynamic {
	temp := float32(36.5)
	voltage := float32(12.0)
	current := float32(2.5)

	bandOptions := NewBandOptions([][]string{
		{"900MHz", "1800MHz", "2100MHz"},
	})

	return &JammerInfoDynamic{
		JammerId:     id,
		Disabled:     false,
		State:        "ACTIVE",
		PositionMode: GeoPositionMode(apiv1.GeoPositionMode_GEO_MANUAL),
		Position: GeoPosition{
			Azimuth: AngleType(45.5),
			Coordinate: GeoCoordinate{
				Lat: 55.7558,
				Lon: 37.6173,
				Alt: &temp,
			},
		},
		Workzone: Workzone{
			Sectors: []WorkzoneSector{
				{
					Number:   1,
					Distance: DistanceType(100.0),
					MinAngle: AngleType(0.0),
					MaxAngle: AngleType(90.0),
				},
				{
					Number:   2,
					Distance: DistanceType(150.0),
					MinAngle: AngleType(90.0),
					MaxAngle: AngleType(180.0),
				},
			},
		},
		Bands: func() JammerBands {
			bands, _ := NewBands(BandList{"900MHz", "1800MHz", "2100MHz"}, BandList{"900MHz", "2100MHz"})
			return bands
		}(),
		BandOptions: &bandOptions,
		HwInfo: &HwInfo{
			Temperature: &temp,
			Voltage:     &voltage,
			Current:     &current,
		},
		TimeoutStatus: &JammerTimeoutStatus{
			Started:  timestamppb.Now().AsTime(),
			Now:      timestamppb.Now().AsTime(),
			Duration: 300000000000,
		},
	}
}
