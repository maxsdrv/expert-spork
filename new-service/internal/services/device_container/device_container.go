package device_container

import (
	"context"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
)

var contErr = core.ProviderErrorFn("device_container")

var logging = ctx_log.GetLogger(nil)

type DeviceContainerService interface {
	ListJammers() []core.DeviceId
	ListSensors() []core.DeviceId
	ListCameras() []core.DeviceId
	ListDevices() []core.DeviceId
	Jammer(id core.DeviceId) (core.JammerBase, error)
	Sensor(id core.DeviceId) (core.SensorBase, error)
	Camera(id core.DeviceId) (core.CameraBase, error)
	Device(id core.DeviceId) (core.DeviceBase, error)
	AppendDevice(id core.DeviceId, device core.DeviceBase) error
}

type deviceContainer struct {
	sensors map[core.DeviceId]core.SensorBase
	jammers map[core.DeviceId]core.JammerBase
	cameras map[core.DeviceId]core.CameraBase
	devices map[core.DeviceId]core.DeviceBase
}

func New(ctx context.Context) DeviceContainerService {
	return &deviceContainer{
		sensors: map[core.DeviceId]core.SensorBase{},
		jammers: map[core.DeviceId]core.JammerBase{},
		cameras: map[core.DeviceId]core.CameraBase{},
		devices: map[core.DeviceId]core.DeviceBase{},
	}
}

func (s *deviceContainer) AppendDevice(id core.DeviceId, device core.DeviceBase) error {
	if err := id.Validate(); err != nil {
		return contErr("%v", err)
	}

	if jammer, ok := device.(core.JammerBase); ok {
		s.jammers[id] = jammer
		s.devices[id] = device

		logging.Debugf("DeviceContainer: added jammer, id %v", id)
	} else if sensor, ok := device.(core.SensorBase); ok {
		s.sensors[id] = sensor
		s.devices[id] = device

		logging.Debugf("DeviceContainer: added sensor, id %v", id)
	} else {
		return contErr("incorrect device type, %s", id)
	}

	return nil
}

func getKeys[T any](m map[core.DeviceId]T) []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(m))
	for id := range m {
		ids = append(ids, id)
	}
	return ids
}

func (s *deviceContainer) ListJammers() []core.DeviceId {
	return getKeys(s.jammers)
}

func (s *deviceContainer) ListSensors() []core.DeviceId {
	return getKeys(s.sensors)
}

func (s *deviceContainer) ListCameras() []core.DeviceId {
	return getKeys(s.cameras)
}

func (s *deviceContainer) ListDevices() []core.DeviceId {
	return getKeys(s.devices)
}

func (s *deviceContainer) Jammer(id core.DeviceId) (core.JammerBase, error) {
	if jammer, ok := s.jammers[id]; ok {
		return jammer, nil
	}
	return nil, contErr("jammer not found, id: %s", id)
}

func (s *deviceContainer) Sensor(id core.DeviceId) (core.SensorBase, error) {
	logging.Debugf("Sensor(): %v", id)
	logging.Debugf("Sensor(): %v", s.sensors)
	if sensor, ok := s.sensors[id]; ok {
		return sensor, nil
	}
	return nil, contErr("sensor not found, id: %s", id)
}

func (s *deviceContainer) Camera(id core.DeviceId) (core.CameraBase, error) {
	logger := logging.WithCtxFields(context.Background())

	logger.Debugf("Camera(): %v", id)

	if camera, ok := s.cameras[id]; ok {
		return camera, nil
	}
	return nil, contErr("camera not found, id: %s", id)
}

func (s *deviceContainer) Device(id core.DeviceId) (core.DeviceBase, error) {
	if device, ok := s.devices[id]; ok {
		return device, nil
	}
	return nil, contErr("device not found, id: %s", id)
}
