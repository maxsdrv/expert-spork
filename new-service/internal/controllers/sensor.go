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

	ids := s.svcDevStorage.ListSensors()

	var sensorInfos []*apiv1.SensorInfo
	for _, sensorId := range ids {
		sensorBase, err := s.svcDevStorage.Sensor(sensorId)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to get sensor %s", sensorId)
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

	if sensorId == "" {
		return controllersError("sensor id is required")
	}

	if timeout < 0 {
		return controllersError("timeout must be non-negative, got: %d", timeout)
	}

	deviceId := core.NewId(sensorId)

	sensorBase, err := s.svcDevStorage.Sensor(deviceId)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get sensor %s", sensorId)
		return err
	}

	if jammerWriter, ok := (*sensorBase).(core.SensorJammerWriter); ok {
		err = jammerWriter.SetJammerMode(jammerMode, timeout)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to set jammer mode for sensor %s", sensorId)
			return err
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
	logger.Debugf("Request data: %s", req.Msg)
	logger.Debugf("Position mode: %s", req.Msg.PositionMode)

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Controllers) SetPosition(
	ctx context.Context,
	req *connect.Request[apiv1.SetPositionRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	deviceId := req.Msg.GetDeviceId()
	if deviceId == "" {
		return nil, controllersError("device id is required")
	}

	position := req.Msg.GetPosition()
	if position == nil {
		return nil, controllersError("position is required")
	}

	corePosition := mapping.

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Controllers) SetDisabled(
	ctx context.Context,
	req *connect.Request[apiv1.SetDisabledRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)
	logger.Debug("Disabled: ", req.Msg.Disabled)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
