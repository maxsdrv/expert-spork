package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type HwInfo apiv1.HwInfo

func NewHwInfo(temperature, voltage, current *string) HwInfo {
	return (HwInfo)(apiv1.HwInfo{
		Temperature: temperature,
		Voltage:     voltage,
		Current:     current,
	})
}
