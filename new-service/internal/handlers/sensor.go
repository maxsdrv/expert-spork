package handlers

import (
	"context"
	"fmt"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Sensors(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Request data: %s", req.Msg)

	return s.controllers.GetSensors(ctx)
}

func (s *Handlers) SensorInfo(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoRequest],
) (*connect.Response[apiv1.SensorInfoResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	sensorId := req.Msg.GetSensorId()
	if sensorId == "" {
		return nil, connect.NewError(connect.CodeInvalidArgument, fmt.Errorf("sensor id is required"))
	}

	info, err := s.controllers.GetSensorInfo(ctx, sensorId)
	if err != nil {
		return nil, err
	}

	return info, nil
}

func (s *Handlers) SensorInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoDynamicRequest],
	rep *connect.ServerStream[apiv1.SensorInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return s.controllers.SensorInfoDynamic(ctx, *req.Msg.SensorId,
		func(sensor *apiv1.SensorInfoDynamicResponse) error {
			return rep.Send(sensor)
		},
	)
}

func (s *Handlers) SetJammerMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
