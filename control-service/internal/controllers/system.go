package controllers

import (
	"context"
	"net/http"

	"dds-control-api/internal/generated/api"
)

func (s *Controllers) Reboot(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("System reboot request")

	if err := s.svcUpdate.StartSystemReboot(ctx); err != nil {
		logger.WithError(err).Error("Failed to start system reboot")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("System reboot started")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) GetRebootStatus(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Get system reboot status request")

	rebootStatus, err := s.svcUpdate.GetRebootStatus(ctx)
	if err != nil {
		logger.WithError(err).Error("Failed to get status reboot system")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Reboot status: %s, error: %v", rebootStatus.Status, rebootStatus.Log)
	return api.Response(http.StatusOK, rebootStatus), nil
}

func (s *Controllers) ShutdownSystem(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("System shutdown request")

	result, err := s.svcUpdate.RunCommand(ctx, "system_shutdown", "")
	if err != nil {
		logger.WithError(err).Error("Failed to start system shutdown")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	if result != "ok" {
		logger.Errorf("System shutdown failed: %s", result)
		return api.Response(http.StatusInternalServerError, nil), nil
	}

	logger.Infof("System shutdown was successful")

	return api.Response(http.StatusOK, nil), nil
}
