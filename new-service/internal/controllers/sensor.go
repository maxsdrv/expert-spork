package controllers

import (
	"context"
	"dds-provider/internal/devices/proxy"
	"fmt"

	"connectrpc.com/connect"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Controllers) GetSensors(
	ctx context.Context,
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Get sensors request")

	ids := s.svcBackend.ListSensors()

	var sensorInfos []*apiv1.SensorInfo

	for _, sensorId := range ids {
		sensorBase, err := s.svcBackend.Sensor(sensorId)
		if err != nil {
			logger.WithError(err).Errorf("Failed to get sensor %s", sensorId)
			continue
		}

		if proxySensor, ok := (*sensorBase).(*proxy.Sensor); ok {
			sensorInfo, err := proxySensor.GetSensorInfo(ctx)
			if err != nil {
				logger.WithError(err).Errorf("Failed to get sensor info for sensor %s", sensorId)
				continue
			}
			sensorInfos = append(sensorInfos, sensorInfo)
		}
	}

	return connect.NewResponse(&apiv1.SensorsResponse{
		SensorInfos: sensorInfos,
	}), nil
}

func (s *Controllers) SensorInfoDynamic(
	ctx context.Context,
	sensorId string,
	sendFunc func(*apiv1.SensorInfoDynamicResponse) error,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Start sensor info dynamic sensor %s", sensorId)

	deviceId, err := core.NewId(sensorId)
	if err != nil {
		return err
	}

	sensorChan, closeFunc, err := s.svcSensorNotifier.Stream(ctx, deviceId)
	if err != nil {
		return err
	}
	defer closeFunc()

	for {
		select {
		case <-ctx.Done():
			return ctx.Err()
		case sensorInfo := <-sensorChan:
			if sensorInfo == nil {
				continue
			}
			logger.Tracef("Sending sensor info dynamic sensor %s", sensorId)
			if err = sendFunc(sensorInfo.ToAPI()); err != nil {
				return err
			}
		}
	}

}

func (s *Controllers) SetJammerMode(ctx context.Context,
	sensorId string,
	jammerMode apiv1.JammerMode,
	timeout int32,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Set jammer mode for sensor %s to %s", sensorId, jammerMode)

	deviceId, err := core.NewId(sensorId)
	if err != nil {
		return connect.NewError(connect.CodeInvalidArgument, err)
	}

	sensorBase, err := s.svcBackend.Sensor(deviceId)
	if err != nil {
		logger.WithError(err).Errorf("Sensor not found %s", sensorId)
		return connect.NewError(connect.CodeNotFound, err)
	}

	if jammerWriter, ok := (*sensorBase).(core.SensorJammerWriter); ok {
		err = jammerWriter.SetJammerMode(core.JammerMode(jammerMode), timeout)
		if err != nil {
			logger.WithError(err).Errorf("Failed to set jammer mode for sensor %s", sensorId)
			return connect.NewError(connect.CodeInternal, err)
		}

		logger.Infof("Successfully set jammer mode for sensor %s to %s", sensorId, jammerMode)
		return nil
	}

	return connect.NewError(connect.CodeInternal, fmt.Errorf("sensor doesnot support jammer mode"))
}
