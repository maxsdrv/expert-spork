package backups

import (
	"context"
	"io"
	"os"
	"path/filepath"
	"strings"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/generated/api"
)

var logging = ctx_log.GetLogger(nil)

const (
	extensionSuffix = ".tar.xz"
)

// MoveCrossDevice
// ORIGIN: https://gist.github.com/var23rav/23ae5d0d4d830aff886c3c970b8f6c6b
// GoLang's os.Rename() gives error "invalid cross-device link" for Docker container with volumes
// MoveCrossDevice() will work moving file between folders across different devices/volumes
func MoveCrossDevice(ctx context.Context, srcPath, destPath string) error {
	logger := logging.WithCtxFields(ctx)
	logger.Debugf("Moving file from %s to %s", srcPath, destPath)

	src, err := os.Open(srcPath)
	if err != nil {
		logger.WithError(err).Error("Couldn't open src file")
		return err
	}
	defer src.Close()

	dest, err := os.Create(destPath)
	if err != nil {
		logger.WithError(err).Error("Couldn't open dest file")
		return err
	}
	defer dest.Close()

	if _, err = io.Copy(dest, src); err != nil {
		logger.WithError(err).Error("Couldn't write dest file")
		return err
	}

	// The copy was successful, so now delete the original file
	if err = os.Remove(srcPath); err != nil {
		logger.WithError(err).Error("Couldn't remove src file")
		return err
	}

	logger.Debugf("Successfully moved file from %s to %s", srcPath, destPath)
	return nil
}

func GetAllBackups(ctx context.Context, backupDir string) ([]api.ListBackups200ResponseInner, error) {
	logger := logging.WithCtxFields(ctx)

	files, err := os.ReadDir(backupDir)
	if err != nil {
		logger.Errorf("Failed to list directory %v", err)
		return nil, err
	}

	var allBackups []api.ListBackups200ResponseInner
	for _, file := range files {
		if !file.IsDir() {
			if strings.HasSuffix(file.Name(), extensionSuffix) {
				filePath := filepath.Join(backupDir, file.Name())
				fileInfo, err := os.Stat(filePath)
				if err != nil {
					logger.WithError(err).Errorf("Failed to get file info for %s", filePath)
					continue
				}

				backup := api.ListBackups200ResponseInner{
					Name: file.Name(),
					Day:  fileInfo.ModTime().Format("2006-01-02"),
				}
				allBackups = append(allBackups, backup)
			}
		}
	}

	return allBackups, nil
}
