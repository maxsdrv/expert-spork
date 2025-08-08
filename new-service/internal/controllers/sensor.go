package controllers

import (
	"context"
	"fmt"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/services/mapping"
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

	deviceId := req.Msg.GetDeviceId()
	if deviceId == "" {
		return nil, controllersError("device id is required")
	}
	positionMode := req.Msg.GetPositionMode()

	deviceIdCore := core.NewId(deviceId)
	deviceBase, err := s.svcDevStorage.Device(deviceIdCore)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get device %s", deviceId)
		return nil, err
	}

	if positionWriter, ok := (*deviceBase).(core.DevicePositionWriter); ok {
		err = positionWriter.SetPositionMode(positionMode)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to set position mode for device %s", deviceId)
			return nil, err
		}
	}

	logger.Infof("Successfully set position mode for device %s", deviceId)
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

	corePosition := mapping.ConvertGeoPosition(position)
	if corePosition == nil {
		return nil, controllersError("invalid position data")
	}

	deviceIdCore := core.NewId(deviceId)
	deviceBase, err := s.svcDevStorage.Device(deviceIdCore)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get device %s", deviceId)
		return nil, err
	}

	if positionWriter, ok := (*deviceBase).(core.DevicePositionWriter); ok {
		err = positionWriter.SetPosition(corePosition)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to set position for device %s", deviceId)
			return nil, err
		}
	}

	logger.Infof("Successfully set position for device %s", deviceId)
	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Controllers) SetDisabled(
	ctx context.Context,
	req *connect.Request[apiv1.SetDisabledRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Request data: %s", req.Msg)

	deviceId := req.Msg.GetDeviceId()
	if deviceId == "" {
		return nil, controllersError("device id is required")
	}

	deviceIdCore := core.NewId(deviceId)
	deviceBase, err := s.svcDevStorage.Device(deviceIdCore)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get device %s", deviceId)
		return nil, err
	}

	if disabledWriter, ok := (*deviceBase).(core.DeviceDisabledWriter); ok {
		disabledWriter.SetDisabled(req.Msg.GetDisabled())

		logger.Infof("Successfully set disabled for device %s", deviceId)

		return connect.NewResponse(&emptypb.Empty{}), nil
	}

	return nil, controllersError("device does not support disabled setting")
}
