package controllers

import (
	"context"
	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Controllers) Targets(
	ctx context.Context,
	sendFunc func(*apiv1.TargetsResponse) error,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Start targets streaming")

	targets, err := s.svcProvider.GetTargets(ctx)
	if err != nil {
		return err
	}

	for _, target := range targets {
		err := sendFunc(target)
		if err != nil {
			return err
		}
	}

	return nil
}
