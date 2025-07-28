package backend

import (
	"context"
	"fmt"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
)

var logging = ctx_log.GetLogger(nil)

type BackendService interface {
	ListJammers() []core.DeviceId
	ListSensors() []core.DeviceId
	ListDevices() []core.DeviceId
	Jammer(id core.DeviceId) (*core.JammerBase, error)
	Sensor(id core.DeviceId) (*core.SensorBase, error)
	Device(id core.DeviceId) (*core.DeviceBase, error)
	AppendDevice(id core.DeviceId, device core.DeviceBase)
}

type backendService struct {
	sensors map[core.DeviceId]*core.SensorBase
	jammers map[core.DeviceId]*core.JammerBase
	devices map[core.DeviceId]*core.DeviceBase
}

func New(ctx context.Context) BackendService {
	return &backendService{
		sensors: map[core.DeviceId]*core.SensorBase{},
		jammers: map[core.DeviceId]*core.JammerBase{},
		devices: map[core.DeviceId]*core.DeviceBase{},
	}
}

func (s *backendService) AppendDevice(id core.DeviceId, device core.DeviceBase) {
	if jammer, ok := device.(core.JammerBase); ok {
		s.jammers[id] = &jammer
		s.devices[id] = &device
	} else if sensor, ok := device.(core.SensorBase); ok {
		s.sensors[id] = &sensor
		s.devices[id] = &device
	} else {
		logging.Errorf("backend: Incorrect device type, %s", id)
	}
}

func getKeys[T any](m map[core.DeviceId]*T) []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(m))
	for id := range m {
		ids = append(ids, id)
	}
	return ids
}

func (s *backendService) ListJammers() []core.DeviceId {
	return getKeys(s.jammers)
}

func (s *backendService) ListSensors() []core.DeviceId {
	return getKeys(s.sensors)
}

func (s *backendService) ListDevices() []core.DeviceId {
	return getKeys(s.devices)
}

func (s *backendService) Jammer(id core.DeviceId) (*core.JammerBase, error) {
	if jammer, ok := s.jammers[id]; ok {
		return jammer, nil
	}
	return nil, fmt.Errorf("jammer not found, %s", id)
}

func (s *backendService) Sensor(id core.DeviceId) (*core.SensorBase, error) {
	logging.Debugf("Sensor(): %v", id)
	logging.Debugf("Sensor(): %v", s.sensors)
	if sensor, ok := s.sensors[id]; ok {
		return sensor, nil
	}
	return nil, fmt.Errorf("sensor not found, %s", id)
}

func (s *backendService) Device(id core.DeviceId) (*core.DeviceBase, error) {
	if device, ok := s.devices[id]; ok {
		return device, nil
	}
	return nil, fmt.Errorf("device not found, %s", id)
}
