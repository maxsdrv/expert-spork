package logs

import (
	"archive/tar"
	"bytes"
	"context"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"regexp"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/ulikunitz/xz"
)

var logging = ctx_log.GetLogger(nil)

const (
	rotatedLogFileExtension = ".xz"
	currentLogFile          = "services.log"
	compressorBufferSize    = 1 * 1024 * 1024 // minimal compression level (1): https://github.com/ulikunitz/xz/issues/42#issuecomment-2040031141
)

var logFilenameRegexp = regexp.MustCompile(`^services\.log-(\d{8})\` + rotatedLogFileExtension + `$`)

type logFileInfo struct {
	Filename string
	Date     time.Time
}

type Service struct {
	logsDir string
}

func New(logsDir string) *Service {
	return &Service{logsDir: logsDir}
}

func parseLogFilename(filename string) (time.Time, error) {
	matches := logFilenameRegexp.FindStringSubmatch(filename)
	if len(matches) != 2 {
		return time.Time{}, fmt.Errorf("does not match the pattern")
	}

	dateStr := matches[1]
	logDate, err := time.Parse("20060102", dateStr)
	if err != nil {
		return time.Time{}, fmt.Errorf("invalid date format: %v", err)
	}

	return logDate, nil
}

func (s *Service) listAll(ctx context.Context) ([]logFileInfo, error) {
	logger := logging.WithCtxFields(ctx)

	var logFiles []logFileInfo

	entries, err := os.ReadDir(s.logsDir)
	if err != nil {
		logger.Errorf("failed to read log directory: %v", err)
		return nil, err
	}

	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		if filepath.Ext(entry.Name()) != rotatedLogFileExtension {
			continue
		}

		date, err := parseLogFilename(entry.Name())
		if err != nil {
			logger.Warnf("skipping file %s: %v", entry.Name(), err)
			continue
		}

		logFiles = append(logFiles, logFileInfo{Filename: entry.Name(), Date: date})
	}

	return logFiles, nil
}

func (s *Service) ListByRange(ctx context.Context, from, to time.Time) ([]string, error) {
	logger := logging.WithCtxFields(ctx)

	allLogs, err := s.listAll(ctx)
	if err != nil {
		logger.Errorf("failed to get logs: %v", err)
		return nil, err
	}

	var files []string

	for _, logFile := range allLogs {
		if (logFile.Date.Equal(from) || logFile.Date.After(from)) && (logFile.Date.Equal(to) || logFile.Date.Before(to)) {
			files = append(files, logFile.Filename)
		}
	}

	year, month, day := to.Date()
	toMidnight := time.Date(year, month, day, 23, 59, 59, 0, time.UTC)
	logger.Tracef("toMidnight: %v", toMidnight)

	if time.Now().UTC().Before(toMidnight) {
		files = append(files, currentLogFile)
	}

	return files, nil
}

func addToArchive(ctx context.Context, tarWriter *tar.Writer, filename string, reader io.Reader, size int64) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Archiving file: %s, size: %d", filename, size)

	header := tar.Header{
		Name: filename,
		Size: size,
		Mode: 0444,
	}

	err := tarWriter.WriteHeader(&header)
	if err != nil {
		return err
	}
	logger.Debugf("Written header, writing data")

	size, err = io.Copy(tarWriter, newReader(ctx, reader, filename, size))
	if err != nil {
		return err
	}
	logger.Debugf("Written data, size: %d", size)
	return nil
}

func (s *Service) ArchiveFiles(ctx context.Context, logFiles []string) (io.Reader, error) {
	logger := logging.WithCtxFields(ctx)

	var files []*os.File
	logger.Infof("Opening log files")
	for _, logFile := range logFiles {
		logger.Debugf("Opening file: %s", logFile)
		file, err := os.Open(filepath.Join(s.logsDir, logFile))
		if err != nil {
			for _, file := range files {
				file.Close()
			}
			return nil, err
		}
		files = append(files, file)
	}

	bufOut, bufIn := io.Pipe()
	go func() {
		archived := tar.NewWriter(bufIn)
		defer func() {
			logger.Debugf("Closing writers")
			for _, file := range files {
				file.Close()
			}
			archived.Close()
			bufIn.Close()
		}()
		for i, logFilename := range logFiles {
			if ctx.Err() != nil {
				logger.WithError(ctx.Err()).Error("Operation interrupted")
				return
			}

			info, err := files[i].Stat()
			logger.Infof("Processing file: %s, size: %d", logFilename, info.Size())
			if err != nil {
				logger.WithError(err).Error("Can't get file size")
				return
			}
			if logFilename == currentLogFile {
				logger.Debugf("Compressing file: %s", logFilename)
				logFilename += rotatedLogFileExtension
				var data bytes.Buffer
				compressed, err := xz.WriterConfig{DictCap: compressorBufferSize}.NewWriter(&data)
				if err != nil {
					logger.WithError(err).Error("Can't create compressor")
					return
				}
				_, err = io.Copy(compressed, newReader(ctx, files[i], currentLogFile, info.Size()))
				compressed.Close()
				if err != nil {
					logger.WithError(err).Error("Can't compress file")
					return
				}
				err = addToArchive(ctx, archived, logFilename, &data, int64(data.Len()))
			} else {
				err = addToArchive(ctx, archived, logFilename, files[i], info.Size())
			}
			if err != nil {
				logger.WithError(err).Error("Can't archive file")
				return
			}
			logger.Debugf("Added file: %s", logFilename)
		}
	}()
	return bufOut, nil
}
