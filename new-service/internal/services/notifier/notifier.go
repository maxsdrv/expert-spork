package notifier

import (
	"context"
	"errors"

	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/teivah/broadcast"

	"dds-provider/internal/core"
)

var logging = ctx_log.GetLogger(nil)

type Message interface {
	Id() core.DeviceId
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
