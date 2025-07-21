package core

import (
	"dds-provider/internal/generated/api/proto"
)

type JammerMode apiv1.JammerMode

type SensorInfoDynamic struct {
	DeviceId          DeviceId
	Disabled          bool
	State             string
	Position          GeoPosition
	PositionMode      GeoPositionMode
	Workzone          DeviceWorkzoneSector
	JammerMode        JammerMode
	JammerAutoTimeout int32
	HwInfo            HwInfo
}

func (s *SensorInfoDynamic) Id() DeviceId {
	return s.DeviceId
}

func (s *SensorInfoDynamic) ToAPI() *apiv1.SensorInfoDynamicResponse {
	apiPosition := (*apiv1.GeoPosition)(&s.Position)
	apiPositionMode := (*apiv1.GeoPositionMode)(&s.PositionMode)
	apiWorkzone := (*apiv1.WorkzoneSector)(&s.Workzone)
	apiJammerMode := (*apiv1.JammerMode)(&s.JammerMode)
	apiHwInfo := (*apiv1.HwInfo)(&s.HwInfo)

	return &apiv1.SensorInfoDynamicResponse{
		Disabled:          &s.Disabled,
		State:             &s.State,
		Position:          apiPosition,
		PositionMode:      apiPositionMode,
		Workzones:         []*apiv1.WorkzoneSector{apiWorkzone},
		JammerMode:        apiJammerMode,
		JammerAutoTimeout: &s.JammerAutoTimeout,
		HwInfo:            apiHwInfo,
	}
}
