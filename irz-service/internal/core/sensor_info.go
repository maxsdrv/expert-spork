package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
	"google.golang.org/protobuf/proto"
)

type JammerMode = apiv1.JammerMode
type SensorType = apiv1.SensorType

type SensorInfo struct {
	SensorId   DeviceId
	JammerIds  *[]DeviceId
	SensorType SensorType
	Model      string
	Serial     *string
	SwVersion  *string
}

func (s *SensorInfo) ToAPI() *apiv1.SensorInfo {
	sensorIdStr := s.SensorId.String()

	var jammerIdStrs []string
	if s.JammerIds != nil {
		for _, id := range *s.JammerIds {
			jammerIdStrs = append(jammerIdStrs, id.String())
		}
	}

	return &apiv1.SensorInfo{
		SensorId:  &sensorIdStr,
		JammerIds: jammerIdStrs,
		Type:      &s.SensorType,
		Model:     proto.String(s.Model),
		Serial:    s.Serial,
		SwVersion: s.SwVersion,
	}
}

type SensorInfoDynamic struct {
	SensorId          DeviceId
	Disabled          bool
	State             string
	Position          GeoPosition
	PositionMode      GeoPositionMode
	Workzone          Workzone
	JammerMode        *JammerMode
	JammerAutoTimeout *int32
	HwInfo            *HwInfo
}

func (s *SensorInfoDynamic) Id() DeviceId {
	return s.SensorId
}

func (s *SensorInfoDynamic) ToAPI() *apiv1.SensorInfoDynamicResponse {
	var apiJammerMode *apiv1.JammerMode
	if s.JammerMode != nil {
		apiJammerMode = (*apiv1.JammerMode)(s.JammerMode)
	}

	var apiHwInfo *apiv1.HwInfo
	if s.HwInfo != nil {
		apiHwInfo = s.HwInfo.ToAPI()
	}

	return &apiv1.SensorInfoDynamicResponse{
		Disabled:          &s.Disabled,
		State:             &s.State,
		Position:          s.Position.ToAPI(),
		PositionMode:      &s.PositionMode,
		Workzones:         s.Workzone.ToAPI(),
		JammerMode:        apiJammerMode,
		JammerAutoTimeout: s.JammerAutoTimeout,
		HwInfo:            apiHwInfo,
	}
}
