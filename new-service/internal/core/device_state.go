package core

type DeviceState uint

const (
	DeviceStateUnavailable DeviceState = iota
	DeviceStateOffDuty
	DeviceStateOk
	DeviceStateFailure
	DeviceStateDisabled
)
