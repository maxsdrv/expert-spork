package proxy_service

import (
	"context"

	"github.com/golang/protobuf/proto"

	apiv1 "dds-provider/internal/generated/api/proto"
)

func (s *Service) GetJammerGroups(ctx context.Context) ([]*apiv1.JammerGroup, error) {
	jammerApi := s.apiClient.JammerAPI

	response, _, err := jammerApi.GetJammerGroups(ctx).Execute()
	if err != nil {
		return nil, serviceError("failed to get jammer groups: %v", err)
	}

	if response == nil || response.JammerGroups == nil {
		return nil, serviceError("jammer groups are not found")
	}

	var groups []*apiv1.JammerGroup
	for _, group := range response.JammerGroups {
		jammerGroup := &apiv1.JammerGroup{
			GroupId: proto.String(group.GetId()),
			Name:    proto.String(group.GetName()),
		}
		groups = append(groups, jammerGroup)
	}

	return groups, nil
}
