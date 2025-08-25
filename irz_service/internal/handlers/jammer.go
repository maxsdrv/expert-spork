package handlers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Jammers(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.JammersResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	jammerInfos := []*apiv1.JammersResponse_JammerInfo{
		{
			JammerId: proto.String("550e8400-e29b-41d4-a716-446655440000"),
			Model:    proto.String("Test Model 1"),
		},
		{
			JammerId: proto.String("6ba7b810-9dad-11d1-80b4-00c04fd430c8"),
			Model:    proto.String("Test Model 2"),
		},
	}

	return connect.NewResponse(&apiv1.JammersResponse{JammerInfos: jammerInfos}), nil
}

func (s *Handlers) JammerInfoDynamic(
	ctx context.Context,
	req *connect.Request[apiv1.JammerInfoRequest],
	rep *connect.ServerStream[apiv1.JammerInfoDynamicResponse],
) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

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
	logger.Debug("Request data: ", req.Msg)

	groups := []*apiv1.JammerGroup{
		{GroupId: proto.String("group1"), Name: proto.String("Group 1")},
		{GroupId: proto.String("group2"), Name: proto.String("Group 2")},
	}

	return connect.NewResponse(&apiv1.GroupsResponse{JammerGroups: groups}), nil
}

func (s *Handlers) SetJammerBands(
	ctx context.Context,
	req *connect.Request[apiv1.SetJammerBandsRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

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
