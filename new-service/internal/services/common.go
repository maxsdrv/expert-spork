package common

import (
	"context"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type CommonService struct {
}

func New(ctx context.Context) *CommonService {
	return &CommonService{}
}
