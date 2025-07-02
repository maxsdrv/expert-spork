package core

import apiv1 "dds-provider/internal/generated/api/proto"

type DeviceIdentifier interface {
	Id() DeviceId
}

type DeviceSerial string
type DeviceFingerprint string

type DeviceData interface {
	Serial() DeviceSerial
	Fingerprint() DeviceFingerprint
}

type DevicePositionReader interface {
	Position() GeoPosition
	PositionMode() apiv1.GeoPositionMode
}

type DevicePositionWriter interface {
	SetPosition(position GeoPosition) error
	SetPositionMode(mode apiv1.GeoPositionMode) error
}

type DeviceDisabledReader interface {
	Disabled() bool
}

type DeviceDisabledWriter interface {
	SetDisabled(disabled bool)
}

type SensorJammerModeWriter interface {
	SetJammerMode(mode apiv1.JammerMode) error
}

type JammerBandsWriter interface {
	SetJammerBands(bands JammerBands, duration int32) error
}

type DeviceReader interface {
	DeviceData
	DeviceDisabledReader
	DevicePositionReader
}

type DeviceWriter interface {
	DeviceDisabledWriter
	DevicePositionWriter
}

type DeviceBase interface {
	DeviceIdentifier
	DeviceWriter
}
type SensorBase interface {
	DeviceBase
	SensorJammerModeWriter
}

type Sensor interface {
	SensorBase
	DeviceReader
}

type SensorProxy SensorBase

type JammerBase interface {
	DeviceBase
	JammerBandsWriter
}

type Jammer interface {
	JammerBase
	DeviceReader
}

type JammerProxy JammerBase
