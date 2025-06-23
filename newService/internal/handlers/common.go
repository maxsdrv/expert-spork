package handlers

import (
	"context"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

func (s *Handlers) Settings(
	ctx context.Context,
	req *connect.Request[apiv1.SettingsRequest],
) (*connect.Response[apiv1.SettingsResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	settings := "{}"
	return connect.NewResponse(&apiv1.SettingsResponse{Settings: &settings}), nil
}
