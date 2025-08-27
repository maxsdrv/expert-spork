package versions

import (
	"context"
	"strings"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/generated/api"
	"dds-control-api/internal/services/update"
)

var logging = ctx_log.GetLogger(nil)

type Service struct {
	RegistryAuth string
	RegistryHost string
	RegistryPath string
	Images       map[string]string
}

func New(ctx context.Context, svcUpdate update.Service) (*Service, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Get credentials")

	result, err := svcUpdate.RunCommand(ctx, "registry_get", "")
	if err != nil {
		logger.WithError(err).Error("Failed run command get registry")
		return nil, err
	}

	parts := strings.Split(result, "*")
	services := &Service{
		RegistryHost: parts[0],
		RegistryPath: parts[1],
		Images:       svcUpdate.GetServicesImage(),
	}

	if services.RegistryHost == "" || services.RegistryPath == "" {
		logger.WithError(err).Errorf("Failed to get registry host and registry path")
		return nil, err
	}
	services.RegistryAuth, err = services.parseDockerConfig(ctx, parts[2])
	if err != nil {
		logger.WithError(err).Errorf("Failed to parse docker config")
		return nil, err
	}

	return services, nil
}

func (s *Service) GetTagsForServices(ctx context.Context) ([]api.ServiceVersions, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("GetTagsForServices")

	var svcVers []api.ServiceVersions

	for serviceName, imageRepo := range s.Images {
		versions, err := s.getImageTags(ctx, imageRepo)
		if err != nil {
			return nil, err
		}
		svcVers = append(svcVers, api.ServiceVersions{
			Name:     serviceName,
			Versions: versions,
		})
	}

	return svcVers, nil
}
