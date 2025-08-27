package handlers

import (
	"context"
	"maps"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/controllers"
	"dds-control-api/internal/generated/api"
)

var logging = ctx_log.GetLogger(nil)

type Handlers struct {
	controllers *controllers.Controllers
}

func New(ctx context.Context, controllers *controllers.Controllers) api.Routes {
	logger := logging.WithCtxFields(ctx)
	routes := api.Routes{}

	maps.Copy(routes, api.NewUploadAPIController(controllers).Routes())
	maps.Copy(routes, api.NewUpdateAPIController(controllers).Routes())
	maps.Copy(routes, api.NewServicesAPIController(controllers).Routes())

	logsRoutes := api.NewLogsAPIController(controllers).Routes()
	routeGetLogs, ok := logsRoutes["GetLogs"]
	if !ok {
		logger.Fatal("GetLogs operation's route not found")
	}
	routeGetLogs.HandlerFunc = (&Handlers{controllers: controllers}).GetLogs
	logsRoutes["GetLogs"] = routeGetLogs
	maps.Copy(routes, logsRoutes)

	maps.Copy(routes, api.NewConfigAPIController(controllers).Routes())
	maps.Copy(routes, api.NewInfoAPIController(controllers).Routes())
	maps.Copy(routes, api.NewBackupsAPIController(controllers).Routes())
	maps.Copy(routes, api.NewSystemAPIController(controllers).Routes())

	return routes
}
