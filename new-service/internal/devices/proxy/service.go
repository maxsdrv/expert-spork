package proxy

import (
	"context"
	"dds-provider/internal/services/backend"
	"encoding/json"
	"fmt"
	"time"

	"github.com/coder/websocket"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/enums"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/notifier"
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
	common         *dss_target_service.CommonAPIService
	sensor         *dss_target_service.SensorAPIService
	host           string
	urlWs          string
	jammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic]
	sensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic]
	sensorMapper   *SensorDataMapper
}

type Service struct {
	devices        map[string]*Device
	backendService backend.BackendService
}

func New(ctx context.Context,
	connections []Connection,
	jammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic],
	sensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic],
	backendService backend.BackendService,
) *Service {
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
			common:         apiClient.CommonAPI,
			sensor:         apiClient.SensorAPI,
			host:           connection.Host,
			jammerNotifier: jammerNotifier,
			sensorNotifier: sensorNotifier,
			sensorMapper:   NewSensorDataMapper(),
		}

		go devices[connection.Host].connect(ctx, connection.PortWs)
	}

	service := &Service{
		devices:        devices,
		backendService: backendService,
	}

	service.discoverAndRegisterSensors(ctx)

	return service
}

func (d *Device) connect(ctx context.Context, portWs int) {
	logger := logging.WithCtxFields(ctx)
	ctx = logger.SetCtxField(ctx, enums.LogFieldHost, d.host)

	retryDelay := time.Second
	maxRetryDelay := 30 * time.Second

	for {
		logger.Debug("Connection started")

		_, _, err := d.common.GetApiVersion(ctx).Execute()
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
					_, _, err := d.common.GetApiVersion(ctx).Execute()
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

		break
	}

	d.urlWs = fmt.Sprintf("ws://%s:%d", d.host, portWs)
	d.startConnection(ctx)
}

func (d *Device) updateLoop(ctx context.Context, conn *websocket.Conn) {
	logger := logging.WithCtxFields(ctx)

	for {
		_, data, err := conn.Read(ctx)
		if err != nil {
			logger.WithError(err).Warn("Failed to read data from DDS target service")
			break
		}

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

		if err = d.processMessage(ctx, msgType, dataRaw); err != nil {
			logger.WithError(err).Errorf("Failed to process message type: %s", msgType)
			continue
		}

		logger.Tracef("Successfully processed message type: %s", msgType)
	}
	logger.Warn("UpdateLoop ended")
}

func (d *Device) processMessage(ctx context.Context, msgType string, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	switch msgType {
	case msgTypeSensorInfo:
		return d.processSensorInfo(ctx, dataRaw)
	case msgTypeLicenseStatus:
		return d.licenseStatusUpdate(ctx, dataRaw)
	default:
		logger.Tracef("Unknown message type: %s", msgType)
		return nil
	}
}

func (d *Device) licenseStatusUpdate(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	var licenseStatus dss_target_service.LicenseStatus

	if err := json.Unmarshal(dataRaw, &licenseStatus); err != nil {
		logger.WithError(err).Error("Unmarshalling message failed")
		return err
	}

	logger.Debugf("License status is: %v %s", licenseStatus.Valid, licenseStatus.Description)

	// TODO: Implement logic for sending license status

	return nil
}

func (s *Service) discoverAndRegisterSensors(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	for _, device := range s.devices {
		sensors, err := device.discoverSensors(ctx)
		if err != nil {
			logger.WithError(err).Errorf("Failed to discover sensors from device %s", device.host)
			continue
		}

		for _, sensorInfo := range sensors {
			deviceId, err := core.NewId(sensorInfo.Id)
			if err != nil {
				logger.WithError(err).Errorf("Failed to create device ID for sensor %s", sensorInfo.Id)
				continue
			}

			proxySensor := NewSensor(sensorInfo.Id, device.sensor)
			s.backendService.AppendDevice(deviceId, proxySensor)

			logger.Infof("Registered sensor %s from device %s", sensorInfo.Id, device.host)
		}
	}
}

func (d *Device) discoverSensors(ctx context.Context) ([]dss_target_service.SensorInfo, error) {
	logger := logging.WithCtxFields(ctx)

	sensorList, resp, err := d.sensor.GetSensors(ctx).Execute()
	if err != nil {
		logger.WithError(err).Error("Failed to get sensors from device")
		return nil, err
	}
	defer resp.Body.Close()

	return sensorList.Sensors, nil
}
