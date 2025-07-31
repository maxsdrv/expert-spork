package components

import (
	"context"

	"github.com/teivah/broadcast"

	"dds-provider/internal/core"
)

var notifierError = core.ProviderErrorFn("notifier")

type Message interface {
	Id() core.DeviceId
}

type Notifier[T Message] struct {
	relays   map[string]*broadcast.Relay[T]
	messages map[string]T
}

func NewNotifier[T Message](ctx context.Context) *Notifier[T] {
	return &Notifier[T]{
		relays:   make(map[string]*broadcast.Relay[T]),
		messages: make(map[string]T),
	}
}

func (s *Notifier[T]) Notify(ctx context.Context, info T) error {
	if err := info.Id().Validate(); err != nil {
		return notifierError("%v", err)
	}

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

	return nil
}

func (s *Notifier[T]) Stream(ctx context.Context, deviceId core.DeviceId) (<-chan T, func(), error) {
	id := deviceId.String()
	if _, ok := s.messages[id]; ok {
		listener := s.relays[id].Listener(1)
		s.relays[id].Broadcast(s.messages[id])

		return listener.Ch(), listener.Close, nil
	}

	return nil, nil, notifierError("notifier for this ID not found: %s", id)
}
