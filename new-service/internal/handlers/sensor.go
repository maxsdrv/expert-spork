package handlers

import (
	"context"

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

	sensorIds, err := s.controllers.GetSensors(ctx)
	if err != nil {
		return nil, err
	}

	return connect.NewResponse(&apiv1.SensorsResponse{
		SensorIdList: sensorIds,
	}), nil
}

func (s *Handlers) SensorInfo(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoRequest],
) (*connect.Response[apiv1.SensorInfoResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&apiv1.SensorInfoResponse{}), nil
}

func (s *Handlers) SensorInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoDynamicRequest],
	rep *connect.ServerStream[apiv1.SensorInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return nil
}

func (s *Handlers) SetJammerMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
