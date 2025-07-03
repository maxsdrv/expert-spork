package target_provider

import (
	"context"

	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func (s *Device) GetSettings(ctx context.Context) (*dss_target_service.Settings, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("GetSettings")

	response, _, err := s.common.GetSettings(ctx).Execute()
	if err != nil {
		logger.WithError(err).Error("Failed to get settings")
		return nil, err
	}

	return response, nil
}
