package target_provider

import (
	"context"
	"fmt"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/teivah/broadcast"

	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/proxy/internal/enums"
)

var logging = ctx_log.GetLogger(nil)

type Connection struct {
	Host     string
	PortHttp int
	PortWs   int
}

const (
	msgTypeSensorInfo = "sensor_info"
	msgTypeJammerInfo = "jammer_info"
)

type Device struct {
	device          *dss_target_service.DeviceAPIService
	common          *dss_target_service.CommonAPIService
	target          *dss_target_service.TargetAPIService
	host            string
	urlWs           string
	sensorInfoRelay *broadcast.Relay[*dss_target_service.SensorInfo]
	jammerInfoRelay *broadcast.Relay[*dss_target_service.JammerInfo]
	targetRelay     *broadcast.Relay[*dss_target_service.TargetData]
}

type Service struct {
	devices map[string]*Device
}

func New(ctx context.Context, connections []Connection) *Service {
	devices := make(map[string]*Device)

	for _, connection := range connections {
		urlRest := fmt.Sprintf("http://%s:%d/api/v1", connection.Host, connection.PortHttp)

		deviceConfiguration := dss_target_service.NewConfiguration()
		deviceConfiguration.Servers = dss_target_service.ServerConfigurations{dss_target_service.ServerConfiguration{URL: urlRest}}
		if logging.IsEnabledFor(ctx_log.TraceLevel) {
			deviceConfiguration.Debug = true
		}
		apiClient := dss_target_service.NewAPIClient(deviceConfiguration)
		devices[connection.Host] = &Device{
			device:          apiClient.DeviceAPI,
			common:          apiClient.CommonAPI,
			target:          apiClient.TargetAPI,
			host:            connection.Host,
			sensorInfoRelay: broadcast.NewRelay[*dss_target_service.SensorInfo](),
			jammerInfoRelay: broadcast.NewRelay[*dss_target_service.JammerInfo](),
			targetRelay:     broadcast.NewRelay[*dss_target_service.TargetData](),
		}

		go devices[connection.Host].connect(ctx, connection.PortWs)
	}

	return &Service{devices: devices}
}

func (s *Device) connect(ctx context.Context, portWs int) {
	logger := logging.WithCtxFields(ctx)
	ctx = logger.SetCtxField(ctx, enums.LogFieldHost, s.host)

	retryDelay := time.Second
	maxRetryDelay := 30 * time.Second

	for {
		logger.Debug("Connection started")

		_, _, err := s.common.GetApiVersion(ctx).Execute()
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
					_, _, err := s.common.GetApiVersion(ctx).Execute()
					logger.Debug("Health check started")
					if err != nil {
						logger.WithError(err).Errorf("failed to health check, retrying in %v ", healthRetryDelay)
						healthRetryDelay = healthRetryDelay * 2
						if healthRetryDelay > healthCheckInterval {
							healthRetryDelay = healthCheckInterval
						}
						continue
					}

					logger.Infof("Health checked")

				case <-ctx.Done():
					timer.Stop()
					return
				}
			}
		}()

		break
	}

	s.urlWs = fmt.Sprintf("ws://%s:%d", s.host, portWs)
	s.startConnection(ctx)
}
