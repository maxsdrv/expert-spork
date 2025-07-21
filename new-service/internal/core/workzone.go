package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type DeviceWorkzoneSector apiv1.WorkzoneSector

func NewDeviceWorkzoneSector(number int, distance float64, minAngle float64, maxAngle float64) DeviceWorkzoneSector {
	num := int32(number)
	return (DeviceWorkzoneSector)(apiv1.WorkzoneSector{
		Number:   &num,
		Distance: &distance,
		MinAngle: &minAngle,
		MaxAngle: &maxAngle,
	})
}

type DeviceWorkzone []DeviceWorkzoneSector

func NewDeviceWorkzone(sectors []DeviceWorkzoneSector) DeviceWorkzone {
	return DeviceWorkzone(sectors)
}
