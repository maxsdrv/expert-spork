package update

import (
	"context"
	"encoding/json"
	"fmt"
)

type ServiceConfig struct {
	Image    string
	Critical bool `json:",omitempty"`
}

func (s *updateService) ServiceConfig(service string) (*ServiceConfig, error) {
	serviceConfig, ok := s.servicesConfig[service]
	if !ok {
		return nil, fmt.Errorf("failed to get service config: %s", service)
	}
	return &serviceConfig, nil
}

func (s *updateService) GetServicesImage() map[string]string {
	services := map[string]string{}
	for service, config := range s.servicesConfig {
		services[service] = config.Image
	}
	return services
}

func (s *updateService) getServicesConfig(ctx context.Context) (map[string]ServiceConfig, error) {
	logger := logging.WithCtxFields(ctx)

	servicesConfig, err := s.RunCommand(ctx, "services_config", "")
	if err != nil {
		logger.WithError(err).Error()
		return nil, fmt.Errorf("failed to get services config: %w", err)
	}

	var result map[string]ServiceConfig
	if err := json.Unmarshal([]byte(servicesConfig), &result); err != nil {
		return nil, fmt.Errorf("failed to parse services config: %w", err)
	}
	return result, nil
}
