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
	"dds-provider/internal/generated/api/proto/apiv1connect"
	"dds-provider/internal/handlers"
	"dds-provider/internal/services"
	"dds-provider/internal/services/device_container"
	"dds-provider/internal/services/proxy_service"
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
	svcDevStorage := device_container.New(ctx)
	svcSensorNotifier, svcJammerNotifier := proxy_service.NewNotifiers(ctx)
	svcProvider := proxy_service.New(ctx, config.Proxy, svcSensorNotifier, svcJammerNotifier, svcDevStorage)

	controllers := controllers.NewControllers(svcCommon, svcDevStorage, svcSensorNotifier, svcJammerNotifier, svcProvider)
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
	path, handler = apiv1connect.NewCameraServiceHandler(handlers, interceptors)
	router.Handle(path, handler)
	path, handler = apiv1connect.NewTargetServiceHandler(handlers, interceptors)
	router.Handle(path, handler)

	logger.Infof("Service at %s", path)
	logger.Infof("Service version %s started at %s", info.VERSION, config.General.Listen)

	shutdown.Wait(ctx, &http.Server{Addr: config.General.Listen, Handler: router})
	logger.Info("Service exiting")
}
