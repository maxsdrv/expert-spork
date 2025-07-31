package proxy

import (
	"context"
	"encoding/json"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/services/parsers"
	"dds-provider/internal/services/wsclient"
)

type NotificationProcessor struct {
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic]
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic]
	sensorMapper   *wsclient.SensorDataMapper
	service        *Service
}

func NewNotificationProcessor(
	jammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	sensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	service *Service,
) *NotificationProcessor {
	return &NotificationProcessor{
		jammerNotifier: jammerNotifier,
		sensorNotifier: sensorNotifier,
		sensorMapper:   wsclient.NewSensorDataMapper(),
		service:        service,
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
		logger.WithError(err).Error("Failed to parse sensor info")
		return err
	}

	deviceId := core.NewId(sensor.Id)

	n.service.registerSensors(ctx, sensor.Id, deviceId, sensor)

	dynamicInfo, err := n.dynamicSensorInfo(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(err).Error("Failed to update sensor info")
		return err
	}

	n.sensorNotifier.Notify(ctx, dynamicInfo)

	logger.Tracef("Successfully updated sensor info")

	return nil
}

func (n *NotificationProcessor) dynamicSensorInfo(ctx context.Context, dataRaw json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	sensorInfoDynamic, err := n.sensorMapper.ConvertToSensorInfoDynamic(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(err).Error("Sensor info dynamic response failed")
		return nil, err
	}

	return sensorInfoDynamic, err
}

func (n *NotificationProcessor) processJammerInfo(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	jammerInfo, err := parsers.ParseJammerInfo(dataRaw)
	if err != nil {
		logger.WithError(err).Error("Failed to parse jammer info")
		return err
	}

	deviceId := core.NewId(jammerInfo.Id)

	n.service.registerJammers(ctx, jammerInfo.Id, deviceId, jammerInfo)
	logger.Tracef("Successfully updated jammer info")
	return nil
}

func (n *NotificationProcessor) licenseStatusUpdate(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	licenseStatus, err := parsers.ParseLicenseStatus(dataRaw)
	if err != nil {
		logger.WithError(err).Error("Failed to parse license status")
		return err
	}

	logger.Debugf("License status is: %v %s", licenseStatus.Valid, licenseStatus.Description)

	// TODO: Implement logic for sending license status

	return nil
}
