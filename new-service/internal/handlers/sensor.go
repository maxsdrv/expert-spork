package handlers

import (
	"context"

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

	sensorIds := []string{
		"550e8400-e29b-41d4-a716-446655440001",
		"550e8400-e29b-41d4-a716-446655440002",
		"550e8400-e29b-41d4-a716-446655440003",
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
