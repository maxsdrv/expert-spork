package controllers

import (
	"context"
	"net/http"
	"os"
	"path/filepath"

	"dds-control-api/internal/config"
	"dds-control-api/internal/generated/api"
	"dds-control-api/internal/services/backups"
)

func (s *Controllers) ListBackups(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("List backups in %s", config.Paths.BackupsDir)

	backupEntries, err := backups.GetAllBackups(ctx, config.Paths.BackupsDir)
	if err != nil {
		logger.WithError(err).Error("Failed to get backup archive")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	return api.Response(http.StatusOK, backupEntries), nil
}

func (s *Controllers) GetBackupArchive(_ context.Context, _ string) (api.ImplResponse, error) {
	// Archive returns nginx
	return api.Response(http.StatusNotImplemented, nil), nil
}

func (s *Controllers) UploadBackup(ctx context.Context, filename, _, _ string) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Upload backup request filename: %s", filename)

	uploadedFile := filepath.Join(config.Paths.UploadDir, filename)
	destFile := filepath.Join(config.Paths.BackupsDir, "restore", filename)

	ctx = logger.SetCtxField(ctx, "file", uploadedFile)

	restoreDir := filepath.Join(config.Paths.BackupsDir, "restore")
	if err := os.MkdirAll(restoreDir, 0755); err != nil {
		logger.WithError(err).Error("Failed to create backup restore directory")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	if err := backups.MoveCrossDevice(ctx, uploadedFile, destFile); err != nil {
		return api.Response(http.StatusInternalServerError, nil), err
	}

	if _, err := s.svcUpdate.RunCommand(ctx, "system_restore", ""); err != nil {
		logger.WithError(err).Errorf("Failed to run system restore")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	return api.Response(http.StatusOK, nil), nil
}
