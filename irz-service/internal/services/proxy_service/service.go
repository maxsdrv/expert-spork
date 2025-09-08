package proxy_service

import (
	"context"
	"fmt"
	"net/http"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/devices/proxy"
	"dds-provider/internal/enums"
	"dds-provider/internal/generated/provider_client"
	"dds-provider/internal/services/device_container"
	"dds-provider/internal/services/wsclient"
)

var logging = ctx_log.GetLogger(nil)

const (
	DefaultPortHttp = 19080
	DefaultPortWs   = 19081
)

const (
	msgTypeSensorInfo    = "sensor_info"
	msgTypeJammerInfo    = "jammer_info"
	msgTypeLicenseStatus = "license_status"
)

type Connection struct {
	Host string
}

type Service struct {
	devStorage            device_container.DeviceContainerService
	httpClient            *http.Client
	notificationClient    *wsclient.WSNotificationClient
	notificationProcessor *NotificationProcessor
	connection            Connection
	apiClient             *provider_client.APIClient
}

func New(ctx context.Context,
	connection Connection,
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	devStorage device_container.DeviceContainerService,
) *Service {
	httpClient := &http.Client{
		Timeout: 30 * time.Second,
		Transport: &http.Transport{
			MaxIdleConns:        10,
			IdleConnTimeout:     60 * time.Second,
			DisableCompression:  false,
			DisableKeepAlives:   false,
			MaxIdleConnsPerHost: 10,
		},
	}

	service := &Service{
		devStorage:            devStorage,
		httpClient:            httpClient,
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

	logger.Debug("Starting proxy proxy")

	s.connect(ctx)
}

func (s *Service) connect(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)
	ctx = logger.SetCtxField(ctx, enums.LogFieldHost, s.connection.Host)

	deviceID := fmt.Sprintf("%s:%d", s.connection.Host, DefaultPortHttp)

	urlRest := fmt.Sprintf("http://%s:%d/api/v1", s.connection.Host, DefaultPortHttp)
	deviceConfiguration := s.createAPIConfiguration(urlRest)
	apiClient := provider_client.NewAPIClient(deviceConfiguration)

	retryDelay := time.Second
	maxRetryDelay := 30 * time.Second

	for {
		logger.Debug("Connection started")

		_, _, err := apiClient.CommonAPI.GetApiVersion(ctx).Execute()
		if err != nil {
			logger.WithError(err).Error("failed to connect to DDS target proxy")

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

		logger.Info("Connected to DDS target proxy")

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

		wsURL := fmt.Sprintf("ws://%s:%d", s.connection.Host, DefaultPortWs)
		wsClient := wsclient.NewWSNotificationClient(wsURL, s.notificationProcessor)
		s.notificationClient = wsClient

		logger.Infof("Starting WebSocket notification client for device %s", deviceID)
		go wsClient.Start(ctx)

		break
	}
}

func (s *Service) createAPIConfiguration(baseUrl string) *provider_client.Configuration {
	config := provider_client.NewConfiguration()
	config.Servers = provider_client.ServerConfigurations{
		provider_client.ServerConfiguration{URL: baseUrl},
	}
	config.HTTPClient = s.httpClient
	return config
}

func (s *Service) registerSensors(ctx context.Context, sensorId string, deviceId core.DeviceId, sensorInfo *provider_client.SensorInfo) {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.devStorage.Sensor(deviceId); err == nil {
		return
	}

	proxySensor := proxy.NewSensor(sensorId, s.apiClient, sensorInfo)
	err := s.devStorage.AppendDevice(deviceId, proxySensor)
	if err != nil {
		logger.WithError(err).Error("failed to add sensor")
		return
	}

	logger.Infof("Registered sensor %s from WebSocket notification", sensorId)
}

func (s *Service) registerJammers(ctx context.Context, jammerId string, deviceId core.DeviceId, jammerInfo *provider_client.JammerInfo) {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.devStorage.Jammer(deviceId); err == nil {
		return
	}

	proxyJammer := proxy.NewJammer(jammerId, s.apiClient, jammerInfo)
	s.devStorage.AppendDevice(deviceId, proxyJammer)

	logger.Infof("Registered Jammer %s from WebSocket notification", jammerId)
}
