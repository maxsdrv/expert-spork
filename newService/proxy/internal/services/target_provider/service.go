package target_provider

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"time"

	"github.com/coder/websocket"
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
	msgTypeSensorInfo    = "sensor_info"
	msgTypeLicenseStatus = "license_status"
)

const (
	fieldMessage = "message"
	fieldData    = "data"
)

type Device struct {
	device          *dss_target_service.DeviceAPIService
	common          *dss_target_service.CommonAPIService
	target          *dss_target_service.TargetAPIService
	host            string
	urlWs           string
	sensorInfoRelay *broadcast.Relay[*dss_target_service.SensorInfo]
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

	err := s.GetSettings(ctx)
	if err != nil {
		logger.WithError(err).Error("Failed to get settings")
	}

	s.urlWs = fmt.Sprintf("ws://%s:%d", s.host, portWs)
	s.startConnection(ctx)
}

func (s *Device) updateLoop(ctx context.Context, conn *websocket.Conn) {
	logger := logging.WithCtxFields(ctx)

	for {
		_, data, err := conn.Read(ctx)
		if err != nil {
			logger.WithError(err).Warn("Failed to read data from DDS target service")
			break
		}

		data = bytes.ReplaceAll(
			data,
			[]byte(`"jammer_mode":"UNDEFINED"`),
			[]byte(`"jammer_mode":"AUTO"`),
		)

		var fields map[string]json.RawMessage
		if err = json.Unmarshal(data, &fields); err != nil {
			logger.WithError(err).Error("Unmarshalling message failed")
			continue
		}

		msgTypeRaw, ok := fields[fieldMessage]
		if !ok {
			logger.Errorf("Missing message field %s", fieldMessage)
			continue
		}
		var msgType string
		if err = json.Unmarshal(msgTypeRaw, &msgType); err != nil {
			logger.WithError(err).Error("Unmarshalling message failed")
			continue
		}

		dataRaw, ok := fields[fieldData]
		if !ok {
			logger.Errorf("Missing message field %s", fieldData)
			continue
		}

		if err = s.processMessage(ctx, msgType, dataRaw); err != nil {
			logger.WithError(err).Errorf("Failed to process message type: %s", msgType)
			continue
		}

		logger.Tracef("Successfully processed message type: %s", msgType)
	}
	logger.Warn("UpdateLoop ended")
}

func (s *Device) processMessage(ctx context.Context, msgType string, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	switch msgType {
	case msgTypeSensorInfo:
		return s.processSensorInfo(ctx, dataRaw)
	case msgTypeLicenseStatus:
		return s.processLicenseStatus(ctx, dataRaw)
	default:
		logger.Tracef("Unknown message type: %s", msgType)
		return nil
	}
}

func (s *Device) processSensorInfo(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	var sensorInfo dss_target_service.SensorInfo

	if err := json.Unmarshal(dataRaw, &sensorInfo); err != nil {
		logger.WithError(err).Error("Unmarshalling message failed")
		return err
	}

	logger.Debugf("Sensor info: %+v", sensorInfo)

	s.sensorInfoRelay.Broadcast(&sensorInfo)

	return nil
}

func (s *Device) processLicenseStatus(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	var licenseStatus dss_target_service.LicenseStatus

	if err := json.Unmarshal(dataRaw, &licenseStatus); err != nil {
		logger.WithError(err).Error("Unmarshalling message failed")
		return err
	}

	logger.Debugf("License status is: %v %s", licenseStatus.Valid, licenseStatus.Description)

	return nil
}
