package mapping

import (
	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func ConvertToDSSGeoPosition(position core.GeoPosition) dss_target_service.GeoPosition {
	coordinate := dss_target_service.GeoCoordinate{
		Latitude:  position.Coordinate.Lat,
		Longitude: position.Coordinate.Lon,
	}

	if position.Coordinate.Alt != nil {
		altValue := float64(*position.Coordinate.Alt)
		coordinate.Altitude = &altValue
	}

	return dss_target_service.GeoPosition{
		Azimuth:    float64(position.Azimuth),
		Coordinate: coordinate,
	}
}

func ConvertToDSSGeoPositionMode(positionMode core.GeoPositionMode) dss_target_service.GeoPositionMode {
	switch positionMode {
	case apiv1.GeoPositionMode_GEO_AUTO:
		return dss_target_service.GEOPOSITIONMODE_AUTO
	case apiv1.GeoPositionMode_GEO_MANUAL:
		return dss_target_service.GEOPOSITIONMODE_MANUAL
	case apiv1.GeoPositionMode_GEO_ALWAYS_MANUAL:
		return dss_target_service.GEOPOSITIONMODE_ALWAYS_MANUAL
	default:
		return dss_target_service.GEOPOSITIONMODE_AUTO
	}
}

func ConvertGeoPosition(apiPosition *apiv1.GeoPosition) *core.GeoPosition {
	if apiPosition == nil {
		return &core.GeoPosition{}
	}

	coord := apiPosition.GetCoordinate()
	if coord == nil {
		return &core.GeoPosition{}
	}

	var altitude *core.DistanceType
	if coord.Altitude != nil {
		alt := *coord.Altitude
		altitude = &alt
	}

	return &core.GeoPosition{
		Azimuth: apiPosition.GetAzimuth(),
		Coordinate: core.GeoCoordinate{
			Lat: coord.GetLatitude(),
			Lon: coord.GetLongitude(),
			Alt: altitude,
		},
	}
}
