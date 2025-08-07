package core

import (
	"google.golang.org/protobuf/proto"

	apiv1 "dds-provider/internal/generated/api/proto"
)

type GeoPosition struct {
	Azimuth    AngleType
	Coordinate GeoCoordinate
}

func (g *GeoPosition) ToAPI() *apiv1.GeoPosition {
	return &apiv1.GeoPosition{
		Azimuth:    proto.Float32(float32(g.Azimuth)),
		Coordinate: g.Coordinate.ToAPI(),
	}
}

//func NewDevicePosition(jsonStr string) DevicePosition {}
