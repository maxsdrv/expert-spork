package mapping

import (
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func convertToAPIPosition(pos *dss_target_service.GeoPosition) *apiv1.GeoPosition {
	if pos == nil {
		return nil
	}

	coordinate := &apiv1.GeoCoordinate{
		Latitude:  &pos.Coordinate.Latitude,
		Longitude: &pos.Coordinate.Longitude,
		Altitude:  pos.Coordinate.Altitude,
	}

	return &apiv1.GeoPosition{
		Azimuth:    &pos.Azimuth,
		Coordinate: coordinate,
	}
}

func convertToAPIPositionMode(posMode *dss_target_service.GeoPositionMode) *apiv1.GeoPositionMode {

	switch *posMode {
	case dss_target_service.GEOPOSITIONMODE_AUTO:
		mode := apiv1.GeoPositionMode_GEO_AUTO
		return &mode
	case dss_target_service.GEOPOSITIONMODE_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_MANUAL
		return &mode
	case dss_target_service.GEOPOSITIONMODE_ALWAYS_MANUAL:
		mode := apiv1.GeoPositionMode_GEO_ALWAYS_MANUAL
		return &mode
	default:
		return nil
	}
}

func convertToAPIWorkZone(dssWorkZone []dss_target_service.WorkzoneSector) []*apiv1.WorkzoneSector {
	if len(dssWorkZone) == 0 {
		return nil
	}

	var workZones []*apiv1.WorkzoneSector
	for _, sector := range dssWorkZone {
		workZoneSector := &apiv1.WorkzoneSector{
			Number:   &sector.Number,
			Distance: &sector.Distance,
			MinAngle: &sector.MinAngle,
			MaxAngle: &sector.MaxAngle,
		}
		workZones = append(workZones, workZoneSector)
	}
	return workZones
}
