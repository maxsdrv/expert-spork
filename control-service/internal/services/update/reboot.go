package update

import (
	"context"
	"fmt"
	"os"
	"path/filepath"
)

type RebootStatus string

const (
	RebootDone  RebootStatus = "done"
	RebootError RebootStatus = "error"
	RebootIdle  RebootStatus = "idle"
)

type SystemRebootStatus struct {
	Status RebootStatus
	Log    *string `json:",omitempty"`
}

func (s *updateService) StartSystemReboot(ctx context.Context) error {
	return s.triggerCommand(ctx, "system_reboot", "")
}

func (s *updateService) GetRebootStatus(ctx context.Context) (*SystemRebootStatus, error) {
	logger := logging.WithCtxFields(ctx)

	statusTriggers := filepath.Join(s.triggersDir, "status_reboot_")

	statusFile := statusTriggers + "error"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		data, err := os.ReadFile(statusFile)
		if err != nil {
			return nil, fmt.Errorf("failed to read %s: %v", statusFile, err)
		}
		log := string(data)
		return &SystemRebootStatus{Status: RebootError, Log: &log}, nil
	}

	statusFile = statusTriggers + "ok"
	if _, err := os.Stat(statusFile); err == nil {
		logger.Debugf("Got status file: %s", statusFile)
		return &SystemRebootStatus{Status: RebootDone}, nil
	}

	return &SystemRebootStatus{Status: RebootIdle}, nil
}
