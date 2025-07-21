package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type Direction apiv1.TargetDirection

func NewDirection(bearing float64, distance float64, elevation float64) Direction {
	return (Direction)(apiv1.TargetDirection{
		Bearing:   &bearing,
		Distance:  &distance,
		Elevation: &elevation,
	})
}
