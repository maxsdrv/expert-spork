package target_provider

import (
	"context"
)

func (s *Device) GetSettings(ctx context.Context) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("GetSettings")

	response, _, err := s.common.GetSettings(ctx).Execute()
	if err != nil {
		logger.WithError(err).Error("Failed to get settings")
		return err
	}

	logger.Debug(response)

	return nil
}
