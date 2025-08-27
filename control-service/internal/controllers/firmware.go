package controllers

import (
	"context"
	"net/http"
	"os"
	"path/filepath"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/config"
	"dds-control-api/internal/generated/api"
	"dds-control-api/internal/services/firmware"
)

var logging = ctx_log.GetLogger(nil)

func (s *Controllers) UploadFirmware(ctx context.Context, filename, _, _ string) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("UploadFirmware request filename: %s", filename)

	uploadedFile := filepath.Join(config.Paths.UploadDir, filename)
	ctx = logger.SetCtxField(ctx, "file", uploadedFile)

	if err := firmware.Extract(ctx, uploadedFile, config.Paths.FirmwareDir); err != nil {
		logger.WithError(err).Errorf("Could not extract firmware file")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	if err := os.Remove(uploadedFile); err != nil {
		logger.WithError(err).Errorf("Could not remove file")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	return api.Response(http.StatusOK, nil), nil
}
