package handlers

import (
	"context"
	"time"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Sensors(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	return s.controllers.Sensors(ctx)
}

func (s *Handlers) SensorInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoDynamicRequest],
	rep *connect.ServerStream[apiv1.SensorInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	sensorId := req.Msg.GetSensorId()

	return s.controllers.SensorInfoDynamic(ctx, sensorId, func(response *apiv1.SensorInfoDynamicResponse) error {
		return rep.Send(response)
	})
}

func (s *Handlers) SetJammerMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	sensorId := req.Msg.GetSensorId()

	jammerMode := req.Msg.GetJammerMode()
	timeout := time.Duration(req.Msg.GetTimeout()) * time.Second

	err := s.controllers.SetJammerMode(ctx, sensorId, jammerMode, timeout)
	if err != nil {
		logger.WithError(err).Error("Set jammer mode failed")
		return nil, err
	}

	logger.Infof("Successfully set jammer mode for sensor %s to %s", sensorId, jammerMode)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
