package controllers

import (
	"context"
	"net/http"

	"dds-control-api/internal/generated/api"
)

func (s *Controllers) Metrics(ctx context.Context) (api.ImplResponse, error) {
	return api.Response(http.StatusOK, s.svcMetrics.Get()), nil
}
