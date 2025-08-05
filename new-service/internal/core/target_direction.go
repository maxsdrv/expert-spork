package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"

	"google.golang.org/protobuf/proto"
)

type TargetDirection struct {
	Bearing   AngleType
	Distance  DistanceType
	Elevation *AngleType
}

func (t *TargetDirection) ToAPI() *apiv1.TargetDirection {
	return &apiv1.TargetDirection{
		Bearing:   proto.Float32(float32(t.Bearing)),
		Distance:  proto.Float32(float32(t.Distance)),
		Elevation: t.Elevation,
	}
}
