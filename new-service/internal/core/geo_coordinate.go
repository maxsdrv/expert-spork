package core

import (
	"dds-provider/internal/generated/api/proto"
)

type GeoCoordinate apiv1.GeoCoordinate

func NewGeoCoordinate(longitude float64, latitude float64) GeoCoordinate {
	return (GeoCoordinate)(apiv1.GeoCoordinate{
		Longitude: &longitude,
		Latitude:  &latitude,
	})
}
