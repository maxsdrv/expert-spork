package proxy_service

import (
	"context"
	"fmt"
	"strings"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/devices/proxy"
	"dds-provider/internal/enums"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/device_storage"
	"dds-provider/internal/services/httpclient"
	"dds-provider/internal/services/wsclient"
)

var logging = ctx_log.GetLogger(nil)

var serviceError = core.ProviderError()

var (
	ErrHTTPTimeout           = serviceError("http timeout")
	ErrHTTPConnectionRefused = serviceError("http connection refused")
	ErrHTTPHostNotFound      = serviceError("host not found")
)

const (
	msgTypeSensorInfo    = "sensor_info"
	msgTypeJammerInfo    = "jammer_info"
	msgTypeLicenseStatus = "license_status"
)

type Connection struct {
	Host     string
	PortHttp int
	PortWs   int
}

type Service struct {
	devStorage            device_storage.DeviceStorageService
	httpClient            *httpclient.SharedHttpClient
	notificationClient    *wsclient.WSNotificationClient
	notificationProcessor *NotificationProcessor
	connection            Connection
	apiClient             *dss_target_service.APIClient
}

func New(ctx context.Context,
	connection Connection,
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	devStorage device_storage.DeviceStorageService,
) *Service {
	sharedHttpClient := httpclient.NewSharedHttpClient()

	service := &Service{
		devStorage:            devStorage,
		httpClient:            sharedHttpClient,
		notificationClient:    nil,
		notificationProcessor: nil,
		connection:            connection,
		apiClient:             nil,
	}

	service.notificationProcessor = NewNotificationProcessor(jammerNotifier, sensorNotifier, service)

	go service.start(ctx)

	return service
}

func (s *Service) start(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Starting proxy")

	s.connect(ctx)
}

func (s *Service) connect(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)
	ctx = logger.SetCtxField(ctx, enums.LogFieldHost, s.connection.Host)

	deviceID := fmt.Sprintf("%s:%d", s.connection.Host, s.connection.PortHttp)

	urlRest := fmt.Sprintf("http://%s:%d/api/v1", s.connection.Host, s.connection.PortHttp)
	deviceConfiguration := s.httpClient.CreateAPIConfiguration(urlRest)
	apiClient := dss_target_service.NewAPIClient(deviceConfiguration)

	retryDelay := time.Second
	maxRetryDelay := 30 * time.Second

	for {
		logger.Debug("Connection started")

		_, _, err := apiClient.CommonAPI.GetApiVersion(ctx).Execute()
		if err != nil {
			var connectionErr error
			if strings.Contains(err.Error(), "connection refused") {
				connectionErr = ErrHTTPConnectionRefused
			} else if strings.Contains(err.Error(), "timeout") {
				connectionErr = ErrHTTPTimeout
			} else if strings.Contains(err.Error(), "no such host") {
				connectionErr = ErrHTTPHostNotFound
			} else {
				connectionErr = serviceError("failed to connect to %s: %v", urlRest, err)
			}

			logger.WithError(connectionErr).Errorf("Failed to connect to DDS target service at %s", urlRest)

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

		s.apiClient = apiClient

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
						var healthErr error
						if strings.Contains(err.Error(), "connection refused") {
							healthErr = ErrHTTPConnectionRefused
						} else if strings.Contains(err.Error(), "timeout") {
							healthErr = ErrHTTPTimeout
						} else {
							healthErr = serviceError("failed to health check for %s: %v", urlRest, err)
						}
						logger.WithError(healthErr).Errorf("Failed to health check, retrying in %v", healthRetryDelay)
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

		wsURL := fmt.Sprintf("ws://%s:%d", s.connection.Host, s.connection.PortWs)
		wsClient := wsclient.NewWSNotificationClient(wsURL, s.notificationProcessor)
		s.notificationClient = wsClient

		logger.Infof("Starting WebSocket notification client for device %s", deviceID)
		go wsClient.Start(ctx)

		break
	}
}

func (s *Service) registerSensors(ctx context.Context, sensorId string, deviceId core.DeviceId, sensorInfo *dss_target_service.SensorInfo) error {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.devStorage.Sensor(deviceId); err == nil {
		return nil
	}

	proxySensor, err := proxy.NewSensor(sensorId, s.apiClient, sensorInfo)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Errorf("Failed to create proxy sensor %s", sensorId)
		return err
	}
	s.devStorage.AppendDevice(deviceId, proxySensor)

	logger.Infof("Registered sensor %s from WebSocket notification", sensorId)
	return nil
}

func (s *Service) registerJammers(ctx context.Context, jammerId string, deviceId core.DeviceId, jammerInfo *dss_target_service.JammerInfo) error {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.devStorage.Jammer(deviceId); err == nil {
		return nil
	}

	proxyJammer, err := proxy.NewJammer(jammerId, s.apiClient, jammerInfo)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Errorf("Failed to create proxy jammer %s", jammerId)
		return err
	}
	s.devStorage.AppendDevice(deviceId, proxyJammer)

	logger.Infof("Registered Jammer %s from WebSocket notification", jammerId)
	return nil
}
