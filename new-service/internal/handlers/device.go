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

	return s.controllers.SetDisabled(ctx, req)
}

func (s *Handlers) SetPositionMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)
	logger.Debug("Position mode: ", req.Msg.PositionMode)

	return s.controllers.SetPositionMode(ctx, req)
}

func (s *Handlers) SetPosition(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return s.controllers.SetPosition(ctx, req)
}
