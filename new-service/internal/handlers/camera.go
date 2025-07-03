package handlers

import (
	"context"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/proto"

	"dds-provider/internal/generated/api/proto"
)

func (s *Handlers) CameraId(
	ctx context.Context,
	req *connect.Request[apiv1.CameraIdRequest],
) (*connect.Response[apiv1.CameraIdResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&apiv1.CameraIdResponse{CameraId: proto.String("camera-001")}), nil
}
