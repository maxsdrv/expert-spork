package core

import (
	"dds-provider/internal/generated/api/proto"
)

type JammerInfo struct {
	DeviceId  DeviceId
	Model     *string
	Serial    *string
	SwVersion *string
	SensorId  *string
	GroupId   *string
}

func (j *JammerInfo) Id() DeviceId {
	return j.DeviceId
}

func (j *JammerInfo) ToAPI() *apiv1.JammerInfoResponse {
	return &apiv1.JammerInfoResponse{
		Model:     j.Model,
		Serial:    j.Serial,
		SwVersion: j.SwVersion,
		SensorId:  j.SensorId,
		GroupId:   j.GroupId,
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
