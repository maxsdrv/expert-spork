package backend

import (
	"context"
	"dds-provider/internal/core"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type BackendService struct {
}

func New(ctx context.Context) *BackendService {
	return &BackendService{}
}

func (s *BackendService) Jammers() []core.DeviceId {
	id1, _ := core.NewId("550e8400-e29b-41d4-a716-446655440000")
	id2, _ := core.NewId("6ba7b810-9dad-11d1-80b4-00c04fd430c8")

	return []core.DeviceId{
		id1,
		id2,
	}
}
