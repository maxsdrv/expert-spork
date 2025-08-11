package handlers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Handlers) Targets(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
	rep *connect.ServerStream[apiv1.TargetsResponse],
) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return s.controllers.Targets(ctx, func(response *apiv1.TargetsResponse) error {
		return rep.Send(response)
	})
}
