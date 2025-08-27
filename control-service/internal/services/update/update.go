package update

import (
	"context"
	"fmt"
	"os"
	"path/filepath"

	"dds-control-api/internal/generated/api"
)

func (s *updateService) StartUpdateFirmware(ctx context.Context) error {
	return s.triggerCommand(ctx, "update_firmware", "")
}

func (s *updateService) StartUpdateOnline(ctx context.Context, version, service string) error {
	return s.triggerCommand(ctx, "update_online", version+" "+service)
}

func (s *updateService) StartUpdateRollback(ctx context.Context) error {
	return s.triggerCommand(ctx, "update_rollback", "")
}

func (s *updateService) GetUpdateStatus(ctx context.Context) (*api.UpdateStatusResult, error) {
	logger := logging.WithCtxFields(ctx)

	statusTriggers := filepath.Join(s.triggersDir, "status_update_")

	statusFile := statusTriggers + "error"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		log, err := os.ReadFile(statusFile)
		if err != nil {
			return nil, fmt.Errorf("failed to read %s: %v", statusFile, err)
		}
		return &api.UpdateStatusResult{Status: api.UPDATESTATUS_ERROR, Log: string(log)}, nil
	}

	statusFile = statusTriggers + "ok"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		return &api.UpdateStatusResult{Status: api.UPDATESTATUS_DONE}, nil
	}

	statusFile = statusTriggers + "self_ok"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		return &api.UpdateStatusResult{Status: api.UPDATESTATUS_ACTIVE}, nil
	}

	statusFile = statusTriggers + "self_started"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		return &api.UpdateStatusResult{Status: api.UPDATESTATUS_PREPARING}, nil
	}

	statusFile = statusTriggers + "backup"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		return &api.UpdateStatusResult{Status: api.UPDATESTATUS_BACKUP}, nil
	}

	return &api.UpdateStatusResult{Status: api.UPDATESTATUS_IDLE}, nil
}

func (s *updateService) CancelUpdate(ctx context.Context) error {
	triggerFile := filepath.Join(s.triggersDir, "handle_update_cancel")
	resultOkFile := triggerFile + "_ok"
	resultErrorFile := triggerFile + "_error"

	_, err := s.runCommand(ctx, triggerFile, "", resultOkFile, resultErrorFile)
	return err
}
