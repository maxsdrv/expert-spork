package service

import (
	"context"
	"fmt"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy"
	"dds-provider/internal/enums"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/httpclient"
	"dds-provider/internal/services/notifier"
	"dds-provider/internal/services/wsclient"
)

var logging = ctx_log.GetLogger(nil)

const (
	msgTypeSensorInfo    = "sensor_info"
	msgTypeLicenseStatus = "license_status"
)

type Connection struct {
	Host     string
	PortHttp int
	PortWs   int
}

type Service struct {
	backendService        backend.BackendService
	httpClient            *httpclient.SharedHttpClient
	notificationClients   map[string]*wsclient.WSNotificationClient
	notificationProcessor *NotificationProcessor
	connections           []Connection
}

func New(ctx context.Context,
	connections []Connection,
	jammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic],
	sensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic],
	backendService backend.BackendService,
) *Service {
	sharedHttpClient := httpclient.NewSharedHttpClient()

	service := &Service{
		backendService:        backendService,
		httpClient:            sharedHttpClient,
		notificationClients:   make(map[string]*wsclient.WSNotificationClient),
		notificationProcessor: nil,
		connections:           connections,
	}

	service.notificationProcessor = NewNotificationProcessor(jammerNotifier, sensorNotifier, service)

	go service.start(ctx)

	return service
}

func (s *Service) start(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Starting proxy service")

	for _, connection := range s.connections {
		go s.connect(ctx, connection)
	}
}

func (s *Service) connect(ctx context.Context, connection Connection) {
	logger := logging.WithCtxFields(ctx)
	ctx = logger.SetCtxField(ctx, enums.LogFieldHost, connection.Host)

	deviceID := fmt.Sprintf("%s:%d", connection.Host, connection.PortHttp)

	urlRest := fmt.Sprintf("http://%s:%d/api/v1", connection.Host, connection.PortHttp)
	deviceConfiguration := s.httpClient.CreateAPIConfiguration(urlRest)
	apiClient := dss_target_service.NewAPIClient(deviceConfiguration)

	retryDelay := time.Second
	maxRetryDelay := 30 * time.Second

	for {
		logger.Debug("Connection started")

		_, _, err := apiClient.CommonAPI.GetApiVersion(ctx).Execute()
		if err != nil {
			logger.WithError(err).Error("failed to connect to DDS target service")

			timer := time.NewTimer(retryDelay)
			select {
			case <-timer.C:
				retryDelay = retryDelay * 2
				if retryDelay > maxRetryDelay {
					retryDelay = maxRetryDelay
				}
			case <-ctx.Done():
				timer.Stop()
				return
			}
			continue
		}

		logger.Info("Connected to DDS target service")

		go func() {
			healthRetryDelay := time.Second
			healthCheckInterval := 60 * time.Second
			for {
				timer := time.NewTimer(healthCheckInterval)
				select {
				case <-timer.C:
					_, _, err := apiClient.CommonAPI.GetApiVersion(ctx).Execute()
					logger.Tracef("Health check started")
					if err != nil {
						logger.WithError(err).Errorf("failed to health check, retrying in %v ", healthRetryDelay)
						healthRetryDelay = healthRetryDelay * 2
						if healthRetryDelay > healthCheckInterval {
							healthRetryDelay = healthCheckInterval
						}
						continue
					}

					logger.Tracef("Health checked")

				case <-ctx.Done():
					timer.Stop()
					return
				}
			}
		}()

		wsURL := fmt.Sprintf("ws://%s:%d", connection.Host, connection.PortWs)
		wsClient := wsclient.NewWSNotificationClient(wsURL, s.notificationProcessor)
		s.notificationClients[deviceID] = wsClient

		logger.Infof("Starting WebSocket notification client for device %s", deviceID)
		go wsClient.Start(ctx)

		break
	}
}

func (s *Service) registerSensors(ctx context.Context, sensorId string, deviceId core.DeviceId, sensorInfo *dss_target_service.SensorInfo) {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.backendService.Sensor(deviceId); err == nil {
		return
	}

	proxySensor := proxy.NewSensor(sensorId, nil, sensorInfo)
	s.backendService.AppendDevice(deviceId, proxySensor)

	logger.Infof("Registered sensor %s from WebSocket notification", sensorId)
}
