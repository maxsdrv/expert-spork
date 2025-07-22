package bulat

import (
	"context"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type BulatService struct {
}

func New(ctx context.Context) *BulatService {
	return &BulatService{}
}
