package controllers

import (
	"context"

	"dds-provider/internal/core"
)

func (s *Controllers) GetCameraId(
	ctx context.Context,
	sensorId string,
) (string, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("GetCameraId request")

	deviceId := core.NewId(sensorId)
	sensorBase, err := s.svcDevStorage.Sensor(deviceId)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get sensor %s", sensorId)
		return "", err
	}

	cameraCapable, ok := sensorBase.(core.CameraReader)
	if !ok {
		return "", controllersError("sensor %s does not support camera id retrieval", sensorId)
	}
	cameraId, err := cameraCapable.GetCamera()
	if err != nil {
		return "", err
	}

	logger.Infof("Successfully get camera id for sensor %s to %s", sensorId, cameraId)

	return cameraId, nil
}
