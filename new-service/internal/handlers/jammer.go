package handlers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Jammers(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.JammersResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	return s.controllers.GetJammers(ctx)
}

func (s *Handlers) JammerInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.JammerInfoRequest],
	rep *connect.ServerStream[apiv1.JammerInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	return s.controllers.JammerInfoDynamic(
		ctx, *req.Msg.JammerId,
		func(jammer *apiv1.JammerInfoDynamicResponse) error {
			return rep.Send(jammer)
		},
	)
}

func (s *Handlers) Groups(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.GroupsResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	groups, err := s.controllers.GetGroups(ctx)
	if err != nil {
		return nil, err
	}
	return connect.NewResponse(&apiv1.GroupsResponse{JammerGroups: groups}), nil
}

func (s *Handlers) SetJammerBands(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerBandsRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	err := s.controllers.SetJammerBands(
		ctx,
		*req.Msg.JammerId,
		req.Msg.BandsActive,
		*req.Msg.Duration,
	)
	if err != nil {
		return nil, err
	}

	return connect.NewResponse(&emptypb.Empty{}), nil
}
