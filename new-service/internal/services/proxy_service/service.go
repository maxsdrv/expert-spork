package proxy_service

import (
	"context"
	"fmt"
	"net/http"
	"strings"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/enums"
	"dds-provider/internal/generated/provider_client"
	"dds-provider/internal/services/device_storage"
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
	devStorage            device_storage.DeviceStorageService
	httpClient            *http.Client
	notificationClient    *wsclient.WSNotificationClient
	notificationProcessor *WsNotification
	connection            Connection
	apiClient             *provider_client.APIClient
}

func New(ctx context.Context,
	connection Connection,
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	devStorage device_storage.DeviceStorageService,
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
		devStorage: devStorage,
		httpClient: httpClient,
		connection: connection,
	}

	go service.start(ctx, jammerNotifier, sensorNotifier)

	return service
}

func (s *Service) start(ctx context.Context,
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Starting proxy")

	s.connect(ctx, jammerNotifier, sensorNotifier)
}

func (s *Service) APIClient() (*provider_client.APIClient, error) {
	if s.apiClient == nil {
		return nil, serviceError("api client is not initialized")
	}
	return s.apiClient, nil
}

func (s *Service) connect(ctx context.Context,
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
) {
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

		s.notificationProcessor = NewWsNotification(jammerNotifier, sensorNotifier, s.devStorage, apiClient)

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
