package wsclient

import (
	"context"
	"encoding/json"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func (d *Device) processSensorInfo(ctx context.Context, dataRaw json.RawMessage) error {
	logger := logging.WithCtxFields(ctx)

	var sensor dss_target_service.SensorInfo
	if err := json.Unmarshal(dataRaw, &sensor); err != nil {
		logger.WithError(err).Error("Unmarshalling message failed")
		return err
	}

	deviceId, err := core.NewId(sensor.Id)
	if err != nil {
		logger.WithError(err).Error("Failed to create device id")
		return err
	}

	dynamicInfo, err := d.dynamicSensorInfo(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(err).Error("Failed to update sensor info")
		return err
	}

	d.sensorNotifier.Notify(ctx, dynamicInfo)

	logger.Tracef("Successfully updated sensor info")

	return nil
}

func (d *Device) dynamicSensorInfo(ctx context.Context, dataRaw json.RawMessage, deviceId core.DeviceId) (*core.SensorInfoDynamic, error) {
	logger := logging.WithCtxFields(ctx)

	sensorInfoDynamic, err := d.sensorMapper.ConvertToSensorInfoDynamic(ctx, dataRaw, deviceId)
	if err != nil {
		logger.WithError(err).Error("Sensor info dynamic response failed")
		return nil, err
	}

	return sensorInfoDynamic, err
}
