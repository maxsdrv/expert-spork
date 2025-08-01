package controllers

import (
	"context"
	"fmt"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

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

		sensorInfo := (*sensorBase).SensorInfo()
		sensorInfos = append(sensorInfos, &sensorInfo)
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

	deviceId := core.NewId(sensorId)

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

	deviceId := core.NewId(sensorId)

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

func (s *Controllers) SetPositionMode(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionModeRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)
	logger.Debug("Position mode: ", req.Msg.PositionMode)

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Controllers) SetPosition(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)
	logger.Debug("Position: ", req.Msg.Position)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
