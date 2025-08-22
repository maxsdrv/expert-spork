package core

import (
	"time"

	apiv1 "dds-provider/internal/generated/api/proto"
)

type DeviceDataReader interface {
	Serial() string
	Fingerprint() string
}

type DevicePositionReader interface {
	Position() GeoPosition
	PositionMode() apiv1.GeoPositionMode
}

type DevicePositionWriter interface {
	SetPosition(position *GeoPosition) error
	SetPositionMode(mode GeoPositionMode) error
}

type DeviceDisabledReader interface {
	Disabled() bool
}

type DeviceDisabledWriter interface {
	SetDisabled(disabled bool) error
}

type SensorJammerReader interface {
	JammerIds() []DeviceId
	JammerMode() JammerMode
	JammerAutoTimeout() int32 //?seconds
}

type SensorJammerWriter interface {
	SetJammerMode(mode JammerMode, timeout time.Duration) error
}

type SensorInfoReader interface {
	SensorInfo() apiv1.SensorInfo
}

type JammerBandsWriter interface {
	SetJammerBands(bands JammerBands, duration int32) error
}

type JammerInfoReader interface {
	JammerInfo() apiv1.JammerInfo
}

type DeviceReader interface {
	DeviceDataReader
	DeviceDisabledReader
	DevicePositionReader
}

type DeviceWriter interface {
	DeviceDisabledWriter
	DevicePositionWriter
}

type DeviceBase interface {
	DeviceWriter
}
type SensorBase interface {
	DeviceBase
	SensorJammerWriter
	SensorInfoReader
}

type Sensor interface {
	SensorBase
	DeviceReader
	SensorJammerReader

	//	SensorInfo() apiv1.SensorInfo
	//
	// SensorInfoDynamic should be composed inside Sensor on every parameter
	// update using some private/global function
}

type SensorProxy SensorBase

type JammerBase interface {
	DeviceBase
	JammerBandsWriter
	JammerInfoReader
}

type Jammer interface {
	JammerBase
	DeviceReader
}

type JammerProxy JammerBase
