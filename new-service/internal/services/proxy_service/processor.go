package proxy_service

import (
	"context"
	"dds-provider/internal/devices/proxy"
	"dds-provider/internal/generated/provider_client"
	"dds-provider/internal/services/device_storage"
	"encoding/json"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/devices/proxy/mapping"
	"dds-provider/internal/services/parsers"
)

type NotificationProcessor struct {
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic]
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic]
	devStorage     device_storage.DeviceStorageService
	apiClient      *provider_client.APIClient
}

func NewNotificationProcessor(
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	devStorage device_storage.DeviceStorageService,
	apiClient *provider_client.APIClient,
) *NotificationProcessor {
	return &NotificationProcessor{
		jammerNotifier: jammerNotifier,
		sensorNotifier: sensorNotifier,
		devStorage:     devStorage,
		apiClient:      apiClient,
	}
}

func (n *NotificationProcessor) HandleNotification(ctx context.Context, message string, data json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	logger.Tracef("Received notification: %s", message)

	switch message {
	case msgTypeSensorInfo:
		return n.processSensorInfo(ctx, data)
	case msgTypeLicenseStatus:
		return n.licenseStatusUpdate(ctx, data)
	case msgTypeJammerInfo:
		return n.processJammerInfo(ctx, data)
	default:
		logger.Tracef("Unknown message type: %s", message)
		return nil
	}
}

func (n *NotificationProcessor) processSensorInfo(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	sensor, err := parsers.ParseSensorInfo(dataRaw)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Failed to parse sensor info")
		return err
	}

	deviceId := core.NewId(sensor.Id)

	if _, err := n.devStorage.Sensor(deviceId); err != nil {
		proxySensor, err := proxy.NewSensor(sensor.Id, n.apiClient, sensor)
		if err != nil {
			logger.WithError(serviceError("%v", err)).Error("Failed to create proxy sensor")
			return err
		}
		n.devStorage.AppendDevice(deviceId, proxySensor)
		logger.Infof("Registered sensor %s from WebSocket notification", sensor.Id)
	}

	dynamicInfo, err := n.dynamicSensorInfo(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Failed to update sensor info")
		return err
	}

	n.sensorNotifier.Notify(ctx, dynamicInfo)

	logger.Tracef("Successfully updated sensor info")

	return nil
}

func (n *NotificationProcessor) dynamicSensorInfo(ctx context.Context, dataRaw json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	sensorInfoDynamic, err := mapping.ConvertToSensorInfoDynamic(dataRaw, deviceId)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Sensor info dynamic response failed")
		return nil, err
	}

	return sensorInfoDynamic, err
}

func (n *NotificationProcessor) processJammerInfo(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	jammerInfo, err := parsers.ParseJammerInfo(dataRaw)
	if err != nil {
		return err
	}

	deviceId := core.NewId(jammerInfo.Id)

	if _, err := n.devStorage.Jammer(deviceId); err != nil {
		proxyJammer, err := proxy.NewJammer(jammerInfo.Id, n.apiClient, jammerInfo)
		if err != nil {
			logger.WithError(serviceError("%v", err)).Error("Failed to create proxy jammer")
			return err
		}
		n.devStorage.AppendDevice(deviceId, proxyJammer)
		logger.Infof("Registered jammer %s from WebSocket notification", jammerInfo.Id)
	}

	dynamicInfo, err := n.dynamicJammerInfo(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Failed to update jammer info")
		return err
	}

	n.jammerNotifier.Notify(ctx, dynamicInfo)

	logger.Tracef("Successfully updated jammer info")

	return nil
}

func (n *NotificationProcessor) dynamicJammerInfo(ctx context.Context, dataRaw json.RawMessage, deviceId core.DeviceId) (*core.JammerInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	jammerInfoDynamic, err := mapping.ConvertToJammerInfoDynamic(dataRaw, deviceId)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Jammer info dynamic response failed")
		return nil, err
	}
	return jammerInfoDynamic, err
}

func (n *NotificationProcessor) licenseStatusUpdate(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	licenseStatus, err := parsers.ParseLicenseStatus(dataRaw)
	if err != nil {
		logger.WithError(serviceError("%v", err)).Error("Failed to parse license status")
		return err
	}

	logger.Debugf("License status is: %v %s", licenseStatus.Valid, licenseStatus.Description)

	// TODO: Implement logic for sending license status

	return nil
}
