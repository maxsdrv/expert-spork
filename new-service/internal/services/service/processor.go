package service

import (
	"context"
	"encoding/json"

	"dds-provider/internal/core"
	"dds-provider/internal/services/notifier"
	"dds-provider/internal/services/parsers"
	"dds-provider/internal/services/wsclient"
)

type NotificationProcessor struct {
	jammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic]
	sensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic]
	sensorMapper   *wsclient.SensorDataMapper
	service        *Service
}

func NewNotificationProcessor(
	jammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic],
	sensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic],
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

	deviceId, err := core.NewId(sensor.Id)
	if err != nil {
		logger.WithError(err).Error("Failed to create device id")
		return err
	}

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
