package controllers

import (
	"context"
	"net/http"

	"dds-control-api/internal/generated/api"
)

func (s *Controllers) UpdateFirmware(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UpdateFirmware request")

	if err := s.svcUpdate.StartUpdateFirmware(ctx); err != nil {
		logger.WithError(err).Errorf("Failed to start firmware update")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Started firmware update")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) UpdateOnline(ctx context.Context, request api.ServiceUpdate) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UpdateOnline request")

	if err := s.svcUpdate.StartUpdateOnline(ctx, request.Version, request.Service); err != nil {
		logger.WithError(err).Errorf("Failed to start online update")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Started online update")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) UpdateRollback(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UpdateRollback request")

	err := s.svcUpdate.StartUpdateRollback(ctx)
	if err != nil {
		logger.WithError(err).Errorf("Failed to start update rollback")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Started update rollback")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) UpdateStatus(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UpdateStatus request")

	status, err := s.svcUpdate.GetUpdateStatus(ctx)
	if err != nil {
		logger.WithError(err).Errorf("Failed to get update status")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Update status: %s, error: %v", status.Status, status.Log)
	return api.Response(http.StatusOK, status), nil
}

func (s *Controllers) UpdateCancel(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UpdateCancel request")

	if err := s.svcUpdate.CancelUpdate(ctx); err != nil {
		logger.WithError(err).Errorf("Failed to cancel update")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Cancelled update")
	return api.Response(http.StatusOK, nil), nil
}
