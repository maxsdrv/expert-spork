package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type SensorInfo struct {
	DeviceId  DeviceId
	Type      *apiv1.SensorType
	Model     *string
	Serial    *string
	SwVersion *string
	JammerIds []string
}

func (s *SensorInfo) Id() DeviceId {
	return s.DeviceId
}

func (s *SensorInfo) ToAPI() *apiv1.SensorInfoResponse {
	return &apiv1.SensorInfoResponse{
		Type:      s.Type,
		Model:     s.Model,
		Serial:    s.Serial,
		SwVersion: s.SwVersion,
		JammerIds: s.JammerIds,
	}
}

type SensorInfoDynamic struct {
	DeviceId     DeviceId
	Disabled     *bool
	State        *string
	Position     *apiv1.GeoPosition
	PositionMode *apiv1.GeoPositionMode
	Workzone     []*apiv1.WorkzoneSector
	HwInfo       *apiv1.HwInfo
}

func (s *SensorInfoDynamic) Id() DeviceId {
	return s.DeviceId
}

func (s *SensorInfoDynamic) ToAPI() *apiv1.SensorInfoDynamicResponse {
	return &apiv1.SensorInfoDynamicResponse{
		Disabled:     s.Disabled,
		State:        s.State,
		Position:     s.Position,
		PositionMode: s.PositionMode,
		Workzone:     s.Workzone,
		HwInfo:       s.HwInfo,
	}
}
