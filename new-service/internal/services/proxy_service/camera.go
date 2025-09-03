package proxy_service

import (
	"context"
)

func (s *Service) GetCamera(ctx context.Context, sensorId string) (string, error) {
	cameraResp, _, err := s.apiClient.CameraAPI.GetId(ctx).Id(sensorId).Execute()
	if err != nil {
		return "", serviceError("failed to get camera id for sensor %s: %v", sensorId, err)
	}

	if cameraResp == nil || cameraResp.CameraId == "" {
		return "", serviceError("camera id is not found for sensor %s", sensorId)
	}

	return cameraResp.CameraId, nil
}
