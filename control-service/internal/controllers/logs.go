package controllers

import (
	"context"
	"net/http"
	"strings"
	"time"

	"dds-control-api/internal/generated/api"
)

func (s *Controllers) ListLogs(
	ctx context.Context,
	from string,
	to string,
) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	fromDate, err := time.Parse("2006-01-02", from)
	if err != nil {
		logger.WithError(err).Error("Failed to parse from date")
		return api.Response(http.StatusBadRequest, nil), err
	}
	toDate, err := time.Parse("2006-01-02", to)
	if err != nil {
		logger.WithError(err).Error("Failed to parse to date")
		return api.Response(http.StatusBadRequest, nil), err
	}

	files, err := s.svcLogs.ListByRange(ctx, fromDate, toDate)
	if err != nil {
		logger.WithError(err).Error("Failed to retrieve logs in range")
		return api.Response(http.StatusBadRequest, nil), err
	}

	if len(files) == 0 {
		logger.Warn("No log files available")
		return api.Response(http.StatusNoContent, nil), nil
	}

	return api.Response(http.StatusOK, files), nil
}

func (s *Controllers) GetLogs(ctx context.Context, files string) (api.ImplResponse, error) {
	filesList := strings.Split(files, ",")
	reader, err := s.svcLogs.ArchiveFiles(ctx, filesList)
	if err != nil {
		return api.ImplResponse{}, err
	}

	return api.ImplResponse{Body: reader}, nil
}
