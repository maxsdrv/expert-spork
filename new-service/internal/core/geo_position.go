package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type GeoPositionMode apiv1.GeoPositionMode
type GeoPosition apiv1.GeoPosition

func NewGeoPosition(longitude float64, latitude float64, azimuth float64) GeoPosition {
	return (GeoPosition)(apiv1.GeoPosition{
		Coordinate: &apiv1.GeoCoordinate{
			Longitude: &longitude,
			Latitude:  &latitude,
		},
		Azimuth: &azimuth,
	})
}
