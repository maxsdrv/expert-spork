package backend

import (
	"context"
	"dds-provider/internal/core"
	"fmt"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type BackendService interface {
	ListJammers() []core.DeviceId
	ListSensors() []core.DeviceId
	ListDevices() []core.DeviceId
	Jammer(id core.DeviceId) (*core.JammerBase, error)
	Sensor(id core.DeviceId) (*core.SensorBase, error)
	Device(id core.DeviceId) (*core.DeviceBase, error)
	AppendDevice(device core.DeviceBase) error
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

func (s *backendService) AppendDevice(device core.DeviceBase) error {
	if jammer, ok := device.(core.JammerBase); ok {
		s.jammers[device.Id()] = &jammer
		s.devices[device.Id()] = &device
		return nil
	} else if sensor, ok := device.(core.SensorBase); ok {
		s.sensors[device.Id()] = &sensor
		s.devices[device.Id()] = &device
		return nil
	} else {
		return fmt.Errorf("incorrect device type, %s", device.Id())
	}
}

func (s *backendService) ListJammers() []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(s.jammers))
	for id := range s.jammers {
		ids = append(ids, id)
	}
	return ids
}

func (s *backendService) ListSensors() []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(s.sensors))
	for id := range s.sensors {
		ids = append(ids, id)
	}
	return ids
}

func (s *backendService) ListDevices() []core.DeviceId {
	ids := make([]core.DeviceId, 0, len(s.devices))
	for id := range s.devices {
		ids = append(ids, id)
	}
	return ids
}

func (s *backendService) Jammer(id core.DeviceId) (*core.JammerBase, error) {
	if jammer, ok := s.jammers[id]; ok {
		return jammer, nil
	}
	return nil, fmt.Errorf("jammer not found, %s", id)
}

func (s *backendService) Sensor(id core.DeviceId) (*core.SensorBase, error) {
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
