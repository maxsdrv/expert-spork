package mapping

import (
	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func convertToPosition(pos dss_target_service.GeoPosition) core.GeoPosition {
	var alt *core.DistanceType
	if pos.Coordinate.Altitude != nil {
		altVal := core.DistanceType(*pos.Coordinate.Altitude)
		alt = &altVal
	}

	coordinate := core.GeoCoordinate{
		Lat: pos.Coordinate.Latitude,
		Lon: pos.Coordinate.Longitude,
		Alt: alt,
	}

	return core.GeoPosition{
		Azimuth:    core.AngleType(pos.Azimuth),
		Coordinate: coordinate,
	}
}

func convertToPositionMode(posMode dss_target_service.GeoPositionMode) core.GeoPositionMode {
	switch posMode {
	case dss_target_service.GEOPOSITIONMODE_AUTO:
		return apiv1.GeoPositionMode_GEO_AUTO
	case dss_target_service.GEOPOSITIONMODE_MANUAL:
		return apiv1.GeoPositionMode_GEO_MANUAL
	case dss_target_service.GEOPOSITIONMODE_ALWAYS_MANUAL:
		return apiv1.GeoPositionMode_GEO_ALWAYS_MANUAL
	default:
		return apiv1.GeoPositionMode_GEO_AUTO
	}
}

func convertToWorkZone(dssWorkZone []dss_target_service.WorkzoneSector) core.Workzone {
	if len(dssWorkZone) == 0 {
		return core.Workzone{}
	}

	var workZones []core.WorkzoneSector
	for _, sector := range dssWorkZone {
		workZoneSector := core.WorkzoneSector{
			Number:   uint32(sector.Number),
			Distance: core.DistanceType(sector.Distance),
			MinAngle: core.AngleType(sector.MinAngle),
			MaxAngle: core.AngleType(sector.MaxAngle),
		}
		workZones = append(workZones, workZoneSector)
	}
	return core.Workzone{Sectors: workZones}
}
