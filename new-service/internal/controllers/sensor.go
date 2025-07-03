package controllers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Controllers) Sensors(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Request data: %s", req.Msg)

	sensorIds := s.svcTargetProvider.GetSensorIds()

	return connect.NewResponse(&apiv1.SensorsResponse{
		SensorIdList: sensorIds,
	}), nil
}

func (s *Controllers) SensorInfo(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoRequest],
) (*connect.Response[apiv1.SensorInfoResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&apiv1.SensorInfoResponse{}), nil
}

func (s *Controllers) SensorInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.SensorInfoDynamicRequest],
	rep *connect.ServerStream[apiv1.SensorInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return nil
}

func (s *Controllers) SetJammerMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
