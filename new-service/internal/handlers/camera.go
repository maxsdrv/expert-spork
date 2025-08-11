package handlers

import (
	"context"

	"connectrpc.com/connect"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) CameraId(
	ctx context.Context,
	req *connect.Request[apiv1.CameraIdRequest],
) (*connect.Response[apiv1.CameraIdResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	sensorId := req.Msg.GetSensorId()

	cameraId, err := s.controllers.GetCameraId(ctx, sensorId)
	if err != nil {
		logger.WithError(err).Error("Get camera id failed")
		return nil, err
	}

	logger.Infof("Successfully get camera id for sensor %s to %s", sensorId, cameraId)

	return connect.NewResponse(&apiv1.CameraIdResponse{CameraId: &cameraId}), nil
}
