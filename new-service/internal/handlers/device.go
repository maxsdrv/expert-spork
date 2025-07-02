package handlers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) SetDisabled(
	ctx context.Context,
	req *connect.Request[apiv1.SetDisabledRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Handlers) PositionMode(
	ctx context.Context,
	req *connect.Request[apiv1.PositionModeRequest],
) (*connect.Response[apiv1.PositionModeResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&apiv1.PositionModeResponse{PositionMode: apiv1.GeoPositionMode_GEO_POSITION_MODE_AUTO.Enum()}), nil
}

func (s *Handlers) SetPositionMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)
	logger.Debug("Position mode: ", req.Msg.PositionMode)

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Handlers) SetPosition(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
