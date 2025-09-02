package controllers

import (
	"context"
)

func (s *Controllers) GetCameraId(
	ctx context.Context,
	sensorId string,
) (string, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("GetCameraId request")

	apiClient, err := s.svcTargetProvider.APIClient()
	if err != nil {
		return "", controllersError("target provider is not configured")
	}

	cameraApi := apiClient.CameraAPI

	cameraIdResponse, _, err := cameraApi.GetId(ctx).Id(sensorId).Execute()
	if err != nil {
		logger.WithError(controllersError("%v", err)).Error("Failed to get camera id")
		return "", err
	}

	if cameraIdResponse == nil || cameraIdResponse.CameraId == "" {
		return "", controllersError("camera id is not found for sensor %s", sensorId)
	}

	cameraId := cameraIdResponse.CameraId

	logger.Infof("Successfully get camera id for sensor %s to %s", sensorId, cameraId)

	return cameraId, nil
}
