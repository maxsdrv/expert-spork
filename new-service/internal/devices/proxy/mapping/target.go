package mapping

import (
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

func ConvertToTargetsResponse(target *provider_client.TargetData) (*apiv1.TargetsResponse, error) {
	alarmStatus, err := convertToAlarmStatus(target.AlarmStatus)
	if err != nil {
		return nil, mappingError("failed to convert to alarm status: %v", err)
	}

	response := &apiv1.TargetsResponse{
		TargetId:    &target.TargetId,
		ClassName:   convertToTargetClass(target.ClassName).Enum(),
		AlarmStatus: alarmStatus,
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

func convertToTargetClass(className provider_client.ClassName) apiv1.TargetClass {
	switch className {
	case provider_client.CLASSNAME_DRONE:
		return apiv1.TargetClass_TARGET_DRONE
	case provider_client.CLASSNAME_CAR:
		return apiv1.TargetClass_TARGET_CAR
	case provider_client.CLASSNAME_TRUCK:
		return apiv1.TargetClass_TARGET_TRUCK
	case provider_client.CLASSNAME_HUMAN:
		return apiv1.TargetClass_TARGET_HUMAN
	case provider_client.CLASSNAME_TREE:
		return apiv1.TargetClass_TARGET_TREE
	case provider_client.CLASSNAME_PEOPLE_GROUP:
		return apiv1.TargetClass_TARGET_PEOPLE_GROUP
	case provider_client.CLASSNAME_MOTORCYCLE:
		return apiv1.TargetClass_TARGET_MOTORCYCLE
	case provider_client.CLASSNAME_JET_SKI:
		return apiv1.TargetClass_TARGET_JET_SKI
	case provider_client.CLASSNAME_BOAT:
		return apiv1.TargetClass_TARGET_BOAT
	case provider_client.CLASSNAME_SHIP:
		return apiv1.TargetClass_TARGET_SHIP
	case provider_client.CLASSNAME_REMOTE_CONSOLE:
		return apiv1.TargetClass_TARGET_REMOTE_CONSOLE
	default:
		return apiv1.TargetClass_TARGET_UNDEFINED
	}
}

func convertToAlarmStatus(alarmStatus provider_client.AlarmStatus) (*apiv1.AlarmStatus, error) {
	alarmLevel, err := convertToAlarmLevel(alarmStatus.Level)
	if err != nil {
		return nil, err
	}
	result := &apiv1.AlarmStatus{
		Level: alarmLevel.Enum(),
	}

	if alarmStatus.Score != nil {
		result.Score = proto.Float64(*alarmStatus.Score)
	}

	return result, nil
}

func convertToAlarmLevel(level string) (apiv1.AlarmLevel, error) {
	switch level {
	case "NONE":
		return apiv1.AlarmLevel_ALARM_NONE, nil
	case "MEDIUM":
		return apiv1.AlarmLevel_ALARM_MEDIUM, nil
	case "HIGH":
		return apiv1.AlarmLevel_ALARM_HIGH, nil
	case "CRITICAL":
		return apiv1.AlarmLevel_ALARM_CRITICAL, nil
	default:
		return apiv1.AlarmLevel_ALARM_NONE, mappingError("unknown AlarmLevel: %v", level)
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
