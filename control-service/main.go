package main

import (
	"context"
	"flag"
	"net/http"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/info"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/config/logging"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/middleware"
	"dev.mpksoft.ru/irz/tamerlan/dds-shared/go/shutdown"

	"dds-control-api/internal/config"
	"dds-control-api/internal/controllers"
	"dds-control-api/internal/handlers"
	"dds-control-api/internal/services/firmware"
	"dds-control-api/internal/services/logs"
	"dds-control-api/internal/services/metrics"
	"dds-control-api/internal/services/update"
	"dds-control-api/internal/services/versions"
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
	if err := firmware.Init(config.CryptPrivateKey, config.SignPublicKey); err != nil {
		logger.Fatal("Identity: ", err)
	}

	ctx := context.Background()

	update, err := update.New(ctx, config.Paths.TriggersDir)
	if err != nil {
		logger.Fatal("Update svc init error: ", err)
	}
	versions, err := versions.New(ctx, update)
	if err != nil {
		logger.Fatal("Versions svc init error: ", err)
	}
	controllers := controllers.New(
		update,
		versions,
		metrics.New(ctx, config.Metrics.NodeExporterHost),
		logs.New(config.Paths.LogsDir),
	)

	routes := handlers.New(ctx, controllers)

	router := http.NewServeMux()
	for operation, handler := range routes {
		endpoint := handler.Method + " " + handler.Pattern
		router.HandleFunc(endpoint, handler.HandlerFunc)
		logger.Debugf("Handling API endpoint \"%s\" as operation %s", endpoint, operation)
	}

	addr := config.General.ListenHost + ":" + config.General.ListenPort
	logger.Infof("Service version %s is listening at %s", info.VERSION, addr)

	shutdown.Wait(ctx, &http.Server{Addr: addr, Handler: middleware.NewLoggingMiddleware(router)})
	logger.Info("Service exiting")
}
