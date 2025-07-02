package core

type DeviceState int32

const (
	DeviceStateUnavailable DeviceState = iota
	DeviceStateOff
	DeviceStateOk
	DeviceStateFailure
)
