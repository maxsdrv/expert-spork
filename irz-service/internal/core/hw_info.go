package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type HwInfo struct {
	Temperature *float32
	Voltage     *float32
	Current     *float32
}

func (h *HwInfo) ToAPI() *apiv1.HwInfo {
	return &apiv1.HwInfo{
		Temperature: h.Temperature,
		Voltage:     h.Voltage,
		Current:     h.Current,
	}
}
