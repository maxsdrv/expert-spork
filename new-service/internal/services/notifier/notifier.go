package notifier

import (
	"context"
	"errors"

	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/teivah/broadcast"
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

type Message interface {
	Id() core.DeviceId
}

func TestJammerInfo(id core.DeviceId) *core.JammerInfo {
	return &core.JammerInfo{
		DeviceId:  id,
		Model:     proto.String("JM-4000"),
		Serial:    proto.String("SN12345678"),
		SwVersion: proto.String("1.2.3"),
		SensorId:  proto.String("9f86d081-884c-4d63-a159-f35e7e5b72a0"),
		GroupId:   proto.String("group1"),
	}
}

func TestJammerInfoDynamic(id core.DeviceId) *core.JammerInfoDynamic {
	return &core.JammerInfoDynamic{
		DeviceId:     id,
		Disabled:     proto.Bool(false),
		State:        proto.String("ACTIVE"),
		PositionMode: apiv1.GeoPositionMode_GEO_POSITION_MODE_MANUAL.Enum(),
		Position: &apiv1.GeoPosition{
			Azimuth: proto.Float64(45.5),
			Coordinate: &apiv1.GeoCoordinate{
				Latitude:  proto.Float64(55.7558),
				Longitude: proto.Float64(37.6173),
				Altitude:  proto.Float64(120.5),
			},
		},
		Workzone: []*apiv1.WorkzoneSector{
			{
				Number:   proto.Int32(1),
				Distance: proto.Float64(100.0),
				MinAngle: proto.Float64(0.0),
				MaxAngle: proto.Float64(90.0),
			},
			{
				Number:   proto.Int32(2),
				Distance: proto.Float64(150.0),
				MinAngle: proto.Float64(90.0),
				MaxAngle: proto.Float64(180.0),
			},
		},
		Bands: []*apiv1.Band{
			{
				Label:  proto.String("GSM"),
				Active: proto.Bool(true),
			},
			{
				Label:  proto.String("LTE"),
				Active: proto.Bool(true),
			},
		},
		BandOptions: []*apiv1.BandOptions{
			{
				Bands: []string{"GSM", "UMTS", "LTE"},
			},
		},
		HwInfo: &apiv1.HwInfo{
			Temperature: proto.String("36.5"),
			Voltage:     proto.String("12.0"),
			Current:     proto.String("2.5"),
		},
		TimeoutStatus: &apiv1.JammerTimeoutStatus{
			Started:  timestamppb.Now(),
			Now:      timestamppb.Now(),
			Duration: proto.Int32(300),
		},
	}
}

type NotifierService[T Message] struct {
	relays   map[string]*broadcast.Relay[T]
	messages map[string]T
}

func New[T Message](ctx context.Context) *NotifierService[T] {
	return &NotifierService[T]{
		relays:   make(map[string]*broadcast.Relay[T]),
		messages: make(map[string]T),
	}
}

func (s *NotifierService[T]) Notify(ctx context.Context, info T) {
	id := info.Id().String()

	if _, ok := s.relays[id]; ok {
		s.messages[id] = info
	} else {
		s.relays[id] = broadcast.NewRelay[T]()
		s.messages[id] = info
	}

	go func() {
		s.relays[id].NotifyCtx(ctx, info)
	}()
}

func (s *NotifierService[T]) Stream(ctx context.Context, deviceId core.DeviceId) (<-chan T, func(), error) {
	id := deviceId.String()
	if _, ok := s.messages[id]; ok {
		listener := s.relays[id].Listener(1)
		s.relays[id].Broadcast(s.messages[id])

		return listener.Ch(), listener.Close, nil
	}

	return nil, nil, errors.New("device not found")
}
