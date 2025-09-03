package mapping

import (
	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

func ConvertToProviderGeoPosition(position core.GeoPosition) provider_client.GeoPosition {
	coordinate := provider_client.GeoCoordinate{
		Latitude:  position.Coordinate.Lat,
		Longitude: position.Coordinate.Lon,
	}

	if position.Coordinate.Alt != nil {
		altValue := float64(*position.Coordinate.Alt)
		coordinate.Altitude = &altValue
	}

	return provider_client.GeoPosition{
		Azimuth:    float64(position.Azimuth),
		Coordinate: coordinate,
	}
}

func ConvertToProviderGeoPositionMode(positionMode core.GeoPositionMode) (provider_client.GeoPositionMode, error) {
	switch positionMode {
	case apiv1.GeoPositionMode_GEO_AUTO:
		return provider_client.GEOPOSITIONMODE_AUTO, nil
	case apiv1.GeoPositionMode_GEO_MANUAL:
		return provider_client.GEOPOSITIONMODE_MANUAL, nil
	case apiv1.GeoPositionMode_GEO_ALWAYS_MANUAL:
		return provider_client.GEOPOSITIONMODE_ALWAYS_MANUAL, nil
	default:
		return provider_client.GEOPOSITIONMODE_UNKNOWN_DEFAULT_OPEN_API, mappingError("to provider geo position mode: %v", positionMode)
	}
}

func ConvertGeoPosition(apiPosition *apiv1.GeoPosition) core.GeoPosition {
	if apiPosition == nil {
		return core.GeoPosition{}
	}

	coord := apiPosition.GetCoordinate()
	if coord == nil {
		return core.GeoPosition{}
	}

	var altitude *core.DistanceType
	if coord.Altitude != nil {
		alt := *coord.Altitude
		altitude = &alt
	}

	return core.GeoPosition{
		Azimuth: apiPosition.GetAzimuth(),
		Coordinate: core.GeoCoordinate{
			Lat: coord.GetLatitude(),
			Lon: coord.GetLongitude(),
			Alt: altitude,
		},
	}
}
