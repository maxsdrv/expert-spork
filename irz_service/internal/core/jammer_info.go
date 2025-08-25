package core

import (
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	"dds-provider/internal/generated/api/proto"
)

type JammerInfo struct {
	DeviceId  DeviceId
	Model     *string
	Serial    *string
	SwVersion *string
	SensorId  DeviceId
	GroupId   *string
}

func (j *JammerInfo) Id() DeviceId {
	return j.DeviceId
}

func (j *JammerInfo) ToAPI() *apiv1.JammersResponse_JammerInfo {
	deviceIdStr := j.DeviceId.String()
	sensorIdStr := j.SensorId.String()
	return &apiv1.JammersResponse_JammerInfo{
		JammerId:  &deviceIdStr,
		Model:     j.Model,
		Serial:    j.Serial,
		SwVersion: j.SwVersion,
		SensorId:  &sensorIdStr,
		GroupId:   j.GroupId,
	}
}

func TestJammerInfo(id DeviceId) *JammerInfo {
	sensorId, _ := NewId("9f86d081-884c-4d63-a159-f35e7e5b72a0")
	return &JammerInfo{
		DeviceId:  id,
		Model:     proto.String("JM-4000"),
		Serial:    proto.String("SN12345678"),
		SwVersion: proto.String("1.2.3"),
		SensorId:  sensorId,
		GroupId:   proto.String("group1"),
	}
}

type JammerInfoDynamic struct {
	DeviceId      DeviceId
	Disabled      *bool
	State         *string
	Position      *apiv1.GeoPosition
	PositionMode  *apiv1.GeoPositionMode
	Workzone      []*apiv1.WorkzoneSector
	Bands         []*apiv1.Band
	BandOptions   []*apiv1.BandOptions
	HwInfo        *apiv1.HwInfo
	TimeoutStatus *apiv1.JammerTimeoutStatus
}

func (j *JammerInfoDynamic) Id() DeviceId {
	return j.DeviceId
}

func (j *JammerInfoDynamic) ToAPI() *apiv1.JammerInfoDynamicResponse {
	return &apiv1.JammerInfoDynamicResponse{
		Disabled:      j.Disabled,
		State:         j.State,
		Position:      j.Position,
		PositionMode:  j.PositionMode,
		Workzone:      j.Workzone,
		Bands:         j.Bands,
		BandOptions:   j.BandOptions,
		HwInfo:        j.HwInfo,
		TimeoutStatus: j.TimeoutStatus,
	}
}

func TestJammerInfoDynamic(id DeviceId) *JammerInfoDynamic {
	return &JammerInfoDynamic{
		DeviceId:     id,
		Disabled:     proto.Bool(false),
		State:        proto.String("ACTIVE"),
		PositionMode: apiv1.GeoPositionMode_GEO_MANUAL.Enum(),
		Position: &apiv1.GeoPosition{
			Azimuth: proto.Float64(45.5),
			Coordinate: &apiv1.GeoCoordinate{
				Latitude:  proto.Float64(55.7558),
				Longitude: proto.Float64(37.6173),
				Altitude:  proto.Float64(120.5),
			},
		},
		Workzone: []*apiv1.WorkzoneSector{
			{
				Number:   proto.Int32(1),
				Distance: proto.Float64(100.0),
				MinAngle: proto.Float64(0.0),
				MaxAngle: proto.Float64(90.0),
			},
			{
				Number:   proto.Int32(2),
				Distance: proto.Float64(150.0),
				MinAngle: proto.Float64(90.0),
				MaxAngle: proto.Float64(180.0),
			},
		},
		Bands: []*apiv1.Band{
			{
				Label:  proto.String("GSM"),
				Active: proto.Bool(true),
			},
			{
				Label:  proto.String("LTE"),
				Active: proto.Bool(true),
			},
		},
		BandOptions: []*apiv1.BandOptions{
			{
				Bands: []string{"GSM", "UMTS", "LTE"},
			},
		},
		HwInfo: &apiv1.HwInfo{
			Temperature: proto.String("36.5"),
			Voltage:     proto.String("12.0"),
			Current:     proto.String("2.5"),
		},
		TimeoutStatus: &apiv1.JammerTimeoutStatus{
			Started:  timestamppb.Now(),
			Now:      timestamppb.Now(),
			Duration: proto.Int32(300),
		},
	}
}
