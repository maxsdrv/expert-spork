package main

import (
	"context"
	"flag"
	"net/http"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/info"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/logging"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/middleware"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/shutdown"

	"dds-provider/internal/config"
	"dds-provider/internal/controllers"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto/apiv1connect"
	"dds-provider/internal/handlers"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/notifier"
)

func main() {
	isDebugMode := flag.Bool("debug", false, "debug mode")
	flag.Parse()

	logging.InitLogger(*isDebugMode, &logging.AppInfo{
		Version: info.VERSION,
		Project: info.PROJECT,
	})
	logger := ctx_log.GetLogger(nil)

	if config.Error() != nil {
		logger.Fatal("Config: ", config.Error())
	}

	ctx := context.Background()

	svcCommon := common.New(ctx)
	svcBackend := backend.New(ctx)
	svcNotifier := notifier.New[*core.JammerInfoDynamic](ctx)

	if id, err := core.NewId("550e8400-e29b-41d4-a716-446655440000"); err == nil {
		svcNotifier.Notify(ctx, notifier.TestJammerInfoDynamic(id))
	} else {
		panic(err)
	}

	controllers := controllers.New(svcCommon, &svcBackend, svcNotifier)
	handlers := handlers.New(controllers)
	interceptors := connect.WithInterceptors(
		middleware.NewLoggingInterceptor(),
		middleware.NewValidateInterceptor(),
	)

	router := http.NewServeMux()
	path, handler := apiv1connect.NewCommonServiceHandler(handlers, interceptors)
	router.Handle(path, handler)
	path, handler = apiv1connect.NewJammerServiceHandler(handlers, interceptors)
	router.Handle(path, handler)
	path, handler = apiv1connect.NewSensorServiceHandler(handlers, interceptors)
	router.Handle(path, handler)
	path, handler = apiv1connect.NewDeviceServiceHandler(handlers, interceptors)
	router.Handle(path, handler)

	logger.Infof("Service at %s", path)
	logger.Infof("Service version %s started at %s", info.VERSION, config.General.Listen)

	shutdown.Wait(ctx, &http.Server{Addr: config.General.Listen, Handler: router})
	logger.Info("Service exiting")
}
