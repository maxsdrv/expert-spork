package controllers

import (
	"context"
	"time"

	"connectrpc.com/connect"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy/mapping"
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

		sensorInfo := sensorBase.SensorInfo()
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

	stream, closer, err := s.svcSensorNotifier.Stream(ctx, deviceId)
	if err != nil {
		return err
	}
	defer closer()

	for {
		select {
		case <-ctx.Done():
			return ctx.Err()
		case sensorInfo := <-stream:
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
	timeout time.Duration,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Set jammer mode for sensor %s to %s", sensorId, jammerMode)

	deviceId := core.NewId(sensorId)

	sensorBase, err := s.svcDevStorage.Sensor(deviceId)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get sensor %s", sensorId)
		return err
	}

	err = sensorBase.SetJammerMode(jammerMode, timeout)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to set jammer mode for sensor %s", sensorId)
		return err
	}

	logger.Infof("Successfully set jammer mode for sensor %s to %s", sensorId, jammerMode)
	return nil
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

	err = deviceBase.SetPositionMode(positionMode)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to set position mode for device %s", deviceId)
		return nil, err
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

	deviceIdCore := core.NewId(deviceId)
	deviceBase, err := s.svcDevStorage.Device(deviceIdCore)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get device %s", deviceId)
		return nil, err
	}

	err = deviceBase.SetPosition(corePosition)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to set position for device %s", deviceId)
		return nil, err
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

	//deviceIdCore := core.NewId(deviceId)
	//deviceBase, err := s.svcDevStorage.Device(deviceIdCore)
	//if err != nil {
	//	logger.WithError(controllersError("%v", err)).Errorf("Failed to get device %s", deviceId)
	//	return nil, err
	//}
	//
	//deviceBase.SetDisabled(req.Msg.GetDisabled())

	logger.Infof("Successfully set disabled for device %s", deviceId)

	return connect.NewResponse(&emptypb.Empty{}), nil

}
