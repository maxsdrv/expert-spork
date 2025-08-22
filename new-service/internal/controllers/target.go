package controllers

import (
	"context"
	"time"

	"dds-provider/internal/devices/proxy/mapping"
	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Controllers) Targets(
	ctx context.Context,
	sendFunc func(*apiv1.TargetsResponse) error,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Start targets streaming")

	apiClient, err := s.svcTargetProvider.APIClient()
	if err != nil {
		return controllersError("target provider is not configured")
	}

	targetApi := apiClient.TargetAPI

	ticker := time.NewTicker(1 * time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			return ctx.Err()
		case <-ticker.C:
			targetList, _, err := targetApi.GetTargets(ctx).Execute()
			if err != nil {
				logger.WithError(controllersError("%v", err)).Error("Failed to get targets")
				continue
			}

			if targetList == nil || targetList.Targets == nil {
				continue
			}

			for _, target := range targetList.Targets {
				targetResponse, err := mapping.ConvertToTargetsResponse(&target)
				if err != nil {
					logger.WithError(controllersError("%v", err)).Error("Failed to convert target")
					continue
				}

				if err = sendFunc(targetResponse); err != nil {
					logger.WithError(controllersError("%v", err)).Error("Failed to send target")
					return err
				}
			}
		}
	}
}
