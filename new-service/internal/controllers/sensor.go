package controllers

import (
	"context"
)

func (s *Controllers) GetSensors(
	ctx context.Context,
) ([]string, error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Get sensors request")

	return []string{}, nil
}
