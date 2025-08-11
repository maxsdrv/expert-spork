package mapping

import (
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

func ConvertToTargetsResponse(target *provider_client.TargetData) (*apiv1.TargetsResponse, error) {
	response := &apiv1.TargetsResponse{
		TargetId:    &target.TargetId,
		ClassName:   convertToTargetClass(string(target.ClassName)).Enum(),
		AlarmStatus: convertToAlarmStatus(target.AlarmStatus),
		LastUpdated: timestamppb.New(target.LastUpdated),
		SensorId:    target.SensorId,
		TrackId:     target.TrackId,
		AlarmIds:    target.AlarmIds,
		ClsIds:      target.ClsIds,
	}

	if target.CameraTrack != nil {
		response.CameraTrack = convertToCameraTrack(*target.CameraTrack)
	}
	if target.Attributes != nil {
		response.Attributes = &apiv1.TargetsResponse_RfdAttributes{
			RfdAttributes: convertToTargetAttributesRFD(*target.Attributes.TargetAttributesRFD),
		}
	}
	if target.Position != nil {
		response.Position = convertToTargetPosition(*target.Position)
	}

	return response, nil
}

func convertToTargetClass(className string) apiv1.TargetClass {
	switch className {
	case "DRONE":
		return apiv1.TargetClass_TARGET_DRONE
	case "CAR":
		return apiv1.TargetClass_TARGET_CAR
	case "TRUCK":
		return apiv1.TargetClass_TARGET_TRUCK
	case "HUMAN":
		return apiv1.TargetClass_TARGET_HUMAN
	case "TREE":
		return apiv1.TargetClass_TARGET_TREE
	case "PEOPLE_GROUP":
		return apiv1.TargetClass_TARGET_PEOPLE_GROUP
	case "MOTORCYCLE":
		return apiv1.TargetClass_TARGET_MOTORCYCLE
	case "JET_SKI":
		return apiv1.TargetClass_TARGET_JET_SKI
	case "BOAT":
		return apiv1.TargetClass_TARGET_BOAT
	case "SHIP":
		return apiv1.TargetClass_TARGET_SHIP
	case "REMOTE_CONSOLE":
		return apiv1.TargetClass_TARGET_REMOTE_CONSOLE
	default:
		return apiv1.TargetClass_TARGET_UNDEFINED
	}
}

func convertToAlarmStatus(alarmStatus provider_client.AlarmStatus) *apiv1.AlarmStatus {
	result := &apiv1.AlarmStatus{
		Level: convertToAlarmLevel(alarmStatus.Level).Enum(),
	}

	if alarmStatus.Score != nil {
		result.Score = proto.Float64(*alarmStatus.Score)
	}

	return result
}

func convertToAlarmLevel(level string) apiv1.AlarmLevel {
	switch level {
	case "NONE":
		return apiv1.AlarmLevel_ALARM_NONE
	case "MEDIUM":
		return apiv1.AlarmLevel_ALARM_MEDIUM
	case "HIGH":
		return apiv1.AlarmLevel_ALARM_HIGH
	case "CRITICAL":
		return apiv1.AlarmLevel_ALARM_CRITICAL
	default:
		return apiv1.AlarmLevel_ALARM_NONE
	}
}

func convertToCameraTrack(cameraTrack provider_client.CameraTrack) *apiv1.CameraTrack {
	return &apiv1.CameraTrack{
		CameraId: &cameraTrack.CameraId,
		TargetId: &cameraTrack.TargetId,
	}
}

func convertToTargetAttributesRFD(attr provider_client.TargetAttributesRFD) *apiv1.TargetAttributesRFD {
	result := &apiv1.TargetAttributesRFD{
		Digital:     &attr.Digital,
		Frequencies: attr.Frequencies,
		Geolocation: &attr.Geolocation,
	}

	if attr.Description != nil {
		result.Description = attr.Description
	}

	return result
}

func convertToTargetPosition(position provider_client.TargetDataPosition) *apiv1.TargetPosition {
	result := &apiv1.TargetPosition{}

	if position.Coordinate != nil {
		result.Coordinate = &apiv1.GeoCoordinate{
			Latitude:  &position.Coordinate.Latitude,
			Longitude: &position.Coordinate.Longitude,
		}

		if position.Coordinate.Altitude != nil {
			result.Coordinate.Altitude = proto.Float32(float32(*position.Coordinate.Altitude))
		}
	}

	if position.SourceId != nil {
		result.SourceId = proto.String(*position.SourceId)
	}

	if position.Direction != nil {
		result.Direction = &apiv1.TargetDirection{
			Bearing:   float64PtrToFloat32(position.Direction.Bearing),
			Distance:  float64PtrToFloat32(position.Direction.Distance),
			Elevation: float64PtrToFloat32(position.Direction.Elevation),
		}
	}

	return result
}

func float64PtrToFloat32(value *float64) *float32 {
	if value == nil {
		return nil
	}
	return proto.Float32(float32(*value))
}
