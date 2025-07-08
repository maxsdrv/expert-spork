package controllers

import (
	"context"

	"connectrpc.com/connect"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Controllers) GetSensors(
	ctx context.Context,
) (*connect.Response[apiv1.SensorsResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Get sensors request")

	ids := s.svcTargetProvider.GetSensorIds()

	return connect.NewResponse(&apiv1.SensorsResponse{
		SensorIdList: ids,
	}), nil
}

func (s *Controllers) GetSensorInfo(ctx context.Context, sensorId string) (*connect.Response[apiv1.SensorInfoResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Get sensor info")

	sensorInfo, err := s.svcTargetProvider.GetSensorInfo(sensorId)
	if err != nil {
		logger.Errorf("Get sensor info error: %v", err)
		return nil, err
	}

	return connect.NewResponse(sensorInfo.ToAPI()), nil
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
