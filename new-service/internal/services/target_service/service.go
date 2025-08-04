package target_service

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
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/httpclient"
	"dds-provider/internal/services/wsclient"
)

var logging = ctx_log.GetLogger(nil)

var serviceError = core.ProviderErrorFn("target service")

var (
	ErrHTTPTimeout            = serviceError("http timeout")
	ErrHTTPConnectionRefused  = serviceError("http connection refused")
	ErrHTTPNetworkUnreachable = serviceError("http network unreachable")
	ErrHTTPHostNotFound       = serviceError("host not found")

	ErrServiceUnavailable = serviceError("service unavailable")
	ErrDeviceNotFound     = serviceError("device not found")
	ErrInvalidDeviceType  = serviceError("invalid device type")
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
	backendService        backend.BackendService
	httpClient            *httpclient.SharedHttpClient
	notificationClients   map[string]*wsclient.WSNotificationClient
	notificationProcessor *NotificationProcessor
	connections           []Connection
	apiClients            map[string]*dss_target_service.APIClient
}

func New(ctx context.Context,
	connections []Connection,
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	backendService backend.BackendService,
) *Service {
	sharedHttpClient := httpclient.NewSharedHttpClient()

	service := &Service{
		backendService:        backendService,
		httpClient:            sharedHttpClient,
		notificationClients:   make(map[string]*wsclient.WSNotificationClient),
		notificationProcessor: nil,
		connections:           connections,
		apiClients:            make(map[string]*dss_target_service.APIClient),
	}

	service.notificationProcessor = NewNotificationProcessor(jammerNotifier, sensorNotifier, service)

	go service.start(ctx)

	return service
}

func (s *Service) start(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Starting proxy")

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
			if strings.Contains(err.Error(), "connection refused") {
				logger.WithError(ErrHTTPConnectionRefused).Errorf("Failed to connect to DDS target service at %s", urlRest)
			} else if strings.Contains(err.Error(), "timeout") {
				logger.WithError(ErrHTTPTimeout).Errorf("Failed to connect to DDS target service at %s", urlRest)
			} else if strings.Contains(err.Error(), "no such host") {
				logger.WithError(ErrHTTPHostNotFound).Errorf("Failed to connect to DDS target service at %s", urlRest)
			} else {
				logger.WithError(serviceError("connection failed to %s: %v", urlRest, err)).Error("API connection failed")
			}

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

		if s.apiClients[deviceID] == nil {
			s.apiClients[deviceID] = apiClient
		}

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
						if strings.Contains(err.Error(), "connection refused") {
							logger.WithError(ErrHTTPConnectionRefused).Errorf("Failed to health check, retrying in %v", healthRetryDelay)
						} else if strings.Contains(err.Error(), "timeout") {
							logger.WithError(ErrHTTPTimeout).Errorf("Failed to health check, retrying in %v", healthRetryDelay)
						} else {
							logger.WithError(serviceError("health check failed for %s: %v", urlRest, err)).Errorf("failed to health check, retrying in %v ", healthRetryDelay)
						}
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

func (s *Service) registerSensors(ctx context.Context, sensorId string, deviceId core.DeviceId, sensorInfo *dss_target_service.SensorInfo) error {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.backendService.Sensor(deviceId); err == nil {
		return nil
	}

	var apiClient *dss_target_service.APIClient
	for _, client := range s.apiClients {
		apiClient = client
		break
	}

	if apiClient == nil {
		return serviceError("No API client for sensor %s", deviceId)
	}

	proxySensor, err := proxy.NewSensor(sensorId, apiClient.SensorAPI, sensorInfo)
	if err != nil {
		logger.WithError(err).Error("failed to create proxy sensor")
		return serviceError("Registration proxy sensor failed for device %s", deviceId)
	}
	s.backendService.AppendDevice(deviceId, proxySensor)

	logger.Infof("Registered sensor %s from WebSocket notification", sensorId)
	return nil
}

func (s *Service) registerJammers(ctx context.Context, jammerId string, deviceId core.DeviceId, jammerInfo *dss_target_service.JammerInfo) error {
	logger := logging.WithCtxFields(ctx)

	if _, err := s.backendService.Jammer(deviceId); err == nil {
		return nil
	}

	var apiClient *dss_target_service.APIClient
	for _, client := range s.apiClients {
		apiClient = client
		break
	}

	if apiClient == nil {
		return serviceError("No API client for jammer %s", deviceId)
	}

	proxyJammer, err := proxy.NewJammer(jammerId, apiClient.JammerAPI, jammerInfo)
	if err != nil {
		logger.WithError(err).Error("failed to create proxy jammer")
		return serviceError("Registration jammer failed for device %s", deviceId)
	}
	s.backendService.AppendDevice(deviceId, proxyJammer)

	logger.Infof("Registered Jammer %s from WebSocket notification", jammerId)
	return nil
}
