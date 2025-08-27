package update

import (
	"context"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/generated/api"
)

type Service interface {
	ServiceConfig(service string) (*ServiceConfig, error)
	GetServicesImage() map[string]string

	StartUpdateFirmware(ctx context.Context) error
	StartUpdateOnline(ctx context.Context, version, service string) error
	StartUpdateRollback(ctx context.Context) error
	GetUpdateStatus(ctx context.Context) (*api.UpdateStatusResult, error)
	CancelUpdate(ctx context.Context) error

	StartSystemReboot(ctx context.Context) error
	GetRebootStatus(ctx context.Context) (*SystemRebootStatus, error)

	RunCommand(ctx context.Context, command string, content string) (string, error)
}

const (
	maxWaitTime  = 20 * time.Second
	pollInterval = 1 * time.Second
)

var logging = ctx_log.GetLogger(nil)

type updateService struct {
	triggersDir    string
	servicesConfig map[string]ServiceConfig
}

func New(ctx context.Context, triggersDir string) (Service, error) {
	svc := &updateService{
		triggersDir: triggersDir,
	}
	var err error
	svc.servicesConfig, err = svc.getServicesConfig(ctx)
	if err != nil {
		return nil, err
	}
	return svc, nil
}

func (s *updateService) RunCommand(ctx context.Context, command string, content string) (string, error) {
	command = "command_" + command
	resultOkFile := filepath.Join(s.triggersDir, "result_ok_"+command)
	resultErrorFile := filepath.Join(s.triggersDir, "result_error_"+command)

	return s.runCommand(ctx, command, content, resultOkFile, resultErrorFile)
}

func (s *updateService) runCommand(ctx context.Context, command, content, resultOkFile, resultErrorFile string) (string, error) {
	logger := logging.WithCtxFields(ctx)

	triggerFile := filepath.Join(s.triggersDir, command)

	cleanupFiles(ctx, triggerFile, resultOkFile, resultErrorFile)

	if err := os.WriteFile(triggerFile, []byte(content), 0644); err != nil {
		return "", err
	}
	logger.Debugf("Created trigger file: %s", triggerFile)

	ticker := time.NewTicker(pollInterval)
	defer ticker.Stop()

	timeout := time.After(maxWaitTime)

	for {
		select {
		case <-timeout:
			logger.Errorf("timed out after %s", maxWaitTime)
			cleanupFiles(ctx, triggerFile, resultOkFile, resultErrorFile)

			return "", fmt.Errorf("timed out waiting for the response")
		case <-ticker.C:
			if _, err := os.Stat(resultOkFile); err == nil {
				logger.Debugf("Got ok file: %s", resultOkFile)
				data, err := os.ReadFile(resultOkFile)
				if err != nil {
					return "", fmt.Errorf("failed to read %s: %v", resultOkFile, err)
				}
				return string(data), nil
			}
			if _, err := os.Stat(resultErrorFile); err == nil {
				logger.Debugf("Got error file: %s", resultErrorFile)
				data, err := os.ReadFile(resultErrorFile)
				if err != nil {
					return "", fmt.Errorf("failed to read %s: %v", resultErrorFile, err)
				}
				return "", fmt.Errorf("command: %s, error: %s", command, string(data))
			}
		}
	}
}

func (s *updateService) triggerCommand(ctx context.Context, command, content string) error {
	logger := logging.WithCtxFields(ctx)

	triggerFile := filepath.Join(s.triggersDir, "command_"+command)

	if err := os.WriteFile(triggerFile, []byte(content), 0644); err != nil {
		return err
	}
	logger.Debugf("Created trigger file: %s", triggerFile)

	ticker := time.NewTicker(pollInterval)
	defer ticker.Stop()

	timeout := time.After(maxWaitTime)

	for {
		select {
		case <-timeout:
			logger.Errorf("timed out after %s", maxWaitTime)
			cleanupFiles(ctx, triggerFile)

			return fmt.Errorf("timed out waiting for the trigger acknowledge")
		case <-ticker.C:
			if _, err := os.Stat(triggerFile); errors.Is(err, os.ErrNotExist) {
				logger.Debugf("Trigger acknowledged successfully: %s", triggerFile)
				return nil
			}
		}
	}
}

func cleanupFiles(ctx context.Context, files ...string) {
	logger := logging.WithCtxFields(ctx)

	for _, file := range files {
		if err := os.Remove(file); err != nil && !os.IsNotExist(err) {
			logger.WithError(err).Errorf("Failed to remove file %s", file)
		}
	}
}
