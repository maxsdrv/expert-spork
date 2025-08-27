package controllers

import (
	"context"
	"errors"
	"fmt"
	"net/http"
	"strings"

	"gopkg.in/yaml.v3"

	"dds-control-api/internal/generated/api"
	"dds-control-api/internal/services/versions"
)

type ServiceInfo struct {
	Service string               `json:"service"`
	Info    *api.ServiceInfoInfo `json:"info,omitempty"`
}

func (s *Controllers) ListServices(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("ListServices request")

	services, err := s.svcUpdate.RunCommand(ctx, "services_list", "")
	if err != nil {
		logger.WithError(err).Error("Failed to get services list")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	var result []ServiceInfo
	if err := yaml.Unmarshal([]byte(services), &result); err != nil {
		logger.WithError(err).Error("Failed to parse services list")
		return api.Response(http.StatusInternalServerError, nil), err
	}
	for i := range result {
		if result[i].Info != nil {
			serviceConfig, err := s.svcUpdate.ServiceConfig(result[i].Service)
			if err != nil {
				return api.Response(http.StatusInternalServerError, nil), err
			}
			result[i].Info.Critical = serviceConfig.Critical
			if result[i].Info.State == "created" {
				result[i].Info.State = api.SERVICESTATE_EXITED
			}
			image := strings.Split(result[i].Info.Version, ":")
			result[i].Info.Version = image[len(image)-1]
		}
	}
	logger.Infof("Services list successfully received")
	return api.Response(http.StatusOK, result), nil
}

func (s *Controllers) SetServiceState(ctx context.Context, request api.SetServiceStateRequest) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("SetServiceState request: %v", request)

	var subcommand string
	switch request.State {
	case api.SERVICESTATE_RUNNING:
		subcommand = "start"
	case api.SERVICESTATE_RESTARTING:
		subcommand = "restart"
	case api.SERVICESTATE_EXITED:
		subcommand = "stop"
	default:
		err := fmt.Errorf("invalid service state: %s", request.State)
		logger.WithError(err).Errorf("Request validation failed")
		return api.Response(http.StatusBadRequest, nil), err
	}
	if _, err := s.svcUpdate.RunCommand(ctx, "services_"+subcommand, request.Service); err != nil {
		logger.WithError(err).Error("Failed to set service state")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Service state set successfully")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) GetServicesVersions(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("GetServicesVersions request")

	tagsResult, err := s.svcVersions.GetTagsForServices(ctx)
	if err != nil {
		var networkConnectionError *versions.NetworkConnectionError
		if errors.As(err, &networkConnectionError) {
			logger.WithError(err).Errorf("Network connection failed")
			return api.Response(http.StatusNoContent, nil), err
		}
		logger.WithError(err).Error("Failed to get services tags")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Service versions received successfully")
	return api.Response(http.StatusOK, tagsResult), nil
}
