package main

import (
	"context"
	"flag"
	"net/http"
	"os"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/info"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/logging"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/middleware"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/shutdown"

	"dds-provider/internal/config"
	"dds-provider/internal/controllers"
	"dds-provider/internal/devices/bulat"
	"dds-provider/internal/generated/api/proto/apiv1connect"
	api "dds-provider/internal/generated/bulat"
	"dds-provider/internal/handlers"
	"dds-provider/internal/services"
	"dds-provider/internal/services/device_container"
	"dds-provider/internal/services/proxy_service"
)

func main() {
	isDebugMode := flag.Bool("debug", false, "debug mode")
	bulatTest := flag.Bool("bulat-test", false, "run Bulat API tests")
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
	svcTargetProvider := proxy_service.New(ctx, config.Proxy, svcSensorNotifier, svcJammerNotifier, svcDevStorage)

	controllers := controllers.NewControllers(svcCommon, svcDevStorage, svcSensorNotifier, svcJammerNotifier, svcTargetProvider)
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

	if *bulatTest {
		logger.Info("Starting Bulat API tests...")

		cfg := api.NewConfiguration()
		cfg.Servers = api.ServerConfigurations{{URL: os.Getenv("BULAT_HOST")}}
		client := api.NewAPIClient(cfg)

		svc, err := bulat.New(ctx, client, os.Getenv("BULAT_TOKEN"))
		if err != nil {
			logger.Fatal(err)
			return
		}

		_, err = svc.GetObjects(ctx)
		if err != nil {
			logger.Errorf("Error getting sensors: %v", err)
		}
		startDeID, err := svc.InitEventID(ctx)
		if err != nil {
			logger.Fatal(err)
		}
		go svc.PollEvents(ctx, startDeID, func(event api.EventListItem) {})

		err = svc.EditObject(ctx, startDeID, "Стационарный объект - 1", 1, 60.192034, 30.9796)
		if err != nil {
			logger.Errorf("Error editing object: %v", err)
		} else {
			logger.Infof("Object edited successfully")
		}

		const testAddr = "0.0.0.0:8080"
		logger.Infof("Bulat test service listening at %s", testAddr)
		shutdown.Wait(ctx, &http.Server{Addr: testAddr, Handler: middleware.NewLoggingMiddleware(http.NewServeMux())})
	} else {
		shutdown.Wait(ctx, &http.Server{Addr: config.General.Listen, Handler: middleware.NewLoggingMiddleware(router)})
	}

	logger.Info("Service exiting")
}
