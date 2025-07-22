package handlers

import (
	"context"
	"fmt"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Sensors(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return s.controllers.GetSensors(ctx)
}

func (s *Handlers) SensorInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoDynamicRequest],
	rep *connect.ServerStream[apiv1.SensorInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	sensorId := req.Msg.GetSensorId()
	if sensorId == "" {
		return connect.NewError(connect.CodeInvalidArgument, fmt.Errorf("sensor id is required"))
	}

	return s.controllers.SensorInfoDynamic(ctx, sensorId, func(response *apiv1.SensorInfoDynamicResponse) error {
		return rep.Send(response)
	})
}

func (s *Handlers) SetJammerMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	sensorId := req.Msg.GetSensorId()
	if sensorId == "" {
		return nil, connect.NewError(connect.CodeInvalidArgument, fmt.Errorf("sensor id is required"))
	}

	jammerMode := req.Msg.GetJammerMode()
	timeout := req.Msg.GetTimeout()

	err := s.controllers.SetJammerMode(ctx, sensorId, jammerMode, timeout)
	if err != nil {
		logger.WithError(err).Error("Set jammer mode failed")
		return nil, err
	}

	logger.Infof("Successfully set jammer mode for sensor %s to %s", sensorId, jammerMode)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
