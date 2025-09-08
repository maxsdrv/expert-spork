package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
	"google.golang.org/protobuf/proto"
)

type AngleType = float32
type CoordinateType = float64
type DistanceType = float32

type GeoCoordinate struct {
	Lon CoordinateType
	Lat CoordinateType
	Alt *DistanceType
}

func (g *GeoCoordinate) ToAPI() *apiv1.GeoCoordinate {
	return &apiv1.GeoCoordinate{
		Longitude: proto.Float64(float64(g.Lon)),
		Latitude:  proto.Float64(float64(g.Lat)),
		Altitude:  g.Alt,
	}
}
