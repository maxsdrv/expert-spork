package proxy_service

import (
	"context"

	"dds-provider/internal/devices/proxy/mapping"
	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Service) GetTargets(ctx context.Context) ([]*apiv1.TargetsResponse, error) {
	targetApi := s.apiClient.TargetAPI

	targetList, _, err := targetApi.GetTargets(ctx).Execute()
	if err != nil {
		return nil, serviceError("failed to get targets: %v", err)
	}

	if targetList == nil || targetList.Targets == nil {
		return []*apiv1.TargetsResponse{}, nil
	}

	var targets []*apiv1.TargetsResponse
	for _, target := range targetList.Targets {
		targetResponse, err := mapping.ConvertToTargetsResponse(&target)
		if err != nil {
			return nil, err
		}
		targets = append(targets, targetResponse)
	}

	return targets, nil
}
