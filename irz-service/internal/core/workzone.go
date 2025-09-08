package core

import (
	"google.golang.org/protobuf/proto"

	apiv1 "dds-provider/internal/generated/api/proto"
)

type WorkzoneSector struct {
	Number   uint
	Distance DistanceType
	MinAngle AngleType
	MaxAngle AngleType
}

func (w *WorkzoneSector) ToAPI() *apiv1.WorkzoneSector {
	return &apiv1.WorkzoneSector{
		Number:   proto.Uint32(uint32(w.Number)),
		Distance: proto.Float32(float32(w.Distance)),
		MinAngle: proto.Float32(float32(w.MinAngle)),
		MaxAngle: proto.Float32(float32(w.MaxAngle)),
	}
}

type Workzone struct {
	Sectors []WorkzoneSector
}

func (w *Workzone) ToAPI() []*apiv1.WorkzoneSector {
	var apiSectors []*apiv1.WorkzoneSector
	for _, sector := range w.Sectors {
		apiSectors = append(apiSectors, sector.ToAPI())
	}
	return apiSectors
}
