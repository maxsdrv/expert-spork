package device_storage

import (
	"context"
	"dds-provider/internal/core"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var devStorageService = core.ProviderErrorFn("device_storage")

var logging = ctx_log.GetLogger(nil)

type DeviceStorageService interface {
	ListJammers() []core.DeviceId
	ListSensors() []core.DeviceId
	ListDevices() []core.DeviceId
	Jammer(id core.DeviceId) (*core.JammerBase, error)
	Sensor(id core.DeviceId) (*core.SensorBase, error)
	Device(id core.DeviceId) (*core.DeviceBase, error)
	AppendDevice(id core.DeviceId, device core.DeviceBase) error
}

type deviceStorage struct {
	sensors map[core.DeviceId]*core.SensorBase
	jammers map[core.DeviceId]*core.JammerBase
	devices map[core.DeviceId]*core.DeviceBase
}

func NewDeviceStorageService(ctx context.Context) DeviceStorageService {
	return &deviceStorage{
		sensors: map[core.DeviceId]*core.SensorBase{},
		jammers: map[core.DeviceId]*core.JammerBase{},
		devices: map[core.DeviceId]*core.DeviceBase{},
	}
}

func (s *deviceStorage) AppendDevice(id core.DeviceId, device core.DeviceBase) error {
	if err := id.Validate(); err != nil {
		return devStorageService("%v", err)
	}

	if jammer, ok := device.(core.JammerBase); ok {
		s.jammers[id] = &jammer
		s.devices[id] = &device
	} else if sensor, ok := device.(core.SensorBase); ok {
		s.sensors[id] = &sensor
		s.devices[id] = &device
	} else {
		return devStorageService("incorrect device type, %s", id)
	}

	return nil
}

func getKeys[T any](m map[core.DeviceId]*T) []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(m))
	for id := range m {
		ids = append(ids, id)
	}
	return ids
}

func (s *deviceStorage) ListJammers() []core.DeviceId {
	return getKeys(s.jammers)
}

func (s *deviceStorage) ListSensors() []core.DeviceId {
	return getKeys(s.sensors)
}

func (s *deviceStorage) ListDevices() []core.DeviceId {
	return getKeys(s.devices)
}

func (s *deviceStorage) Jammer(id core.DeviceId) (*core.JammerBase, error) {
	if jammer, ok := s.jammers[id]; ok {
		return jammer, nil
	}
	return nil, devStorageService("jammer not found, id: %s", id)
}

func (s *deviceStorage) Sensor(id core.DeviceId) (*core.SensorBase, error) {
	logging.Debugf("Sensor(): %v", id)
	logging.Debugf("Sensor(): %v", s.sensors)
	if sensor, ok := s.sensors[id]; ok {
		return sensor, nil
	}
	return nil, devStorageService("sensor not found, id: %s", id)
}

func (s *deviceStorage) Device(id core.DeviceId) (*core.DeviceBase, error) {
	if device, ok := s.devices[id]; ok {
		return device, nil
	}
	return nil, devStorageService("device not found, id: %s", id)
}
