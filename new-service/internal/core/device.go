package core

import apiv1 "dds-provider/internal/generated/api/proto"

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
	SetDisabled(disabled bool)
}

type SensorJammerReader interface {
	JammerIds() []DeviceId
	JammerMode() JammerMode
	JammerAutoTimeout() int32 //?seconds
}

type SensorJammerWriter interface {
	SetJammerMode(mode JammerMode, timeout int32) error
}

type JammerBandsWriter interface {
	SetJammerBands(bands JammerBands, duration int32) error
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
}

type Sensor interface {
	SensorBase
	DeviceReader
	SensorJammerReader

	SensorInfo() apiv1.SensorInfo
	// SensorInfoDynamic should be composed inside Sensor on every parameter
	// update using some private/global function
}

type SensorProxy SensorBase

type JammerBase interface {
	DeviceBase
	JammerBandsWriter
}

type Jammer interface {
	JammerBase
	DeviceReader

	JammerInfo() apiv1.JammerInfo
}

type JammerProxy JammerBase
