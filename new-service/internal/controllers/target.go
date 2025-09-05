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
		if target == nil {
			continue
		}
		if err := sendFunc(target); err != nil {
			return err
		}
	}

	stream, closer := s.svcProvider.SubscribeTargets()
	defer closer()

	for {
		select {
		case <-ctx.Done():
			logger.Debug("Targets streaming stopped: context done")
			return ctx.Err()
		case upd := <-stream:
			if upd == nil {
				continue
			}
			if err := sendFunc(upd); err != nil {
				return err
			}
		}
	}
}
