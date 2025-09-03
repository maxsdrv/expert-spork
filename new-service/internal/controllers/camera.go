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

	cameraId, err := s.svcProvider.GetCamera(ctx, sensorId)
	if err != nil {
		return "", err
	}

	logger.Infof("Successfully get camera id for sensor %s to %s", sensorId, cameraId)

	return cameraId, nil
}
