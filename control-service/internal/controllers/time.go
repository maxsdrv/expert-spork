package controllers

import (
	"context"
	"fmt"
	"net/http"
	"strconv"
	"strings"
	"time"

	"dds-control-api/internal/generated/api"
)

type NtpSyncState struct {
	Enabled bool `json:"enabled"`
}

func (s *Controllers) GetNtpServers(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("GetNtp servers request")

	result, err := s.svcUpdate.RunCommand(ctx, "ntp_get", "")
	if err != nil {
		logger.WithError(err).Errorf("Failed to get ntp servers")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	servers := strings.Fields(result)
	logger.Infof("Successfully retrieved NTP servers: %v", servers)
	return api.Response(http.StatusOK, api.NtpServers{Servers: servers}), nil
}

func (s *Controllers) SetNtpServers(ctx context.Context, servers api.NtpServers) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("SetNtp servers request: %s", servers.Servers)

	ntpServers := strings.Join(servers.Servers, " ")
	if _, err := s.svcUpdate.RunCommand(ctx, "ntp_set", ntpServers); err != nil {
		logger.WithError(err).Error("Failed to set ntp servers")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("NTP configuration updated successfully")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) GetNtpSync(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Get NTP sync request")

	result, err := s.svcUpdate.RunCommand(ctx, "get_ntp_sync", "")
	if err != nil {
		logger.WithError(err).Error("Failed to get NTP sync state")
		return api.Response(http.StatusInternalServerError, nil), err
	}
	var enabled bool
	if result == "yes" {
		enabled = true
	} else if result == "no" {
		enabled = false
	} else {
		err := fmt.Errorf("Failed to parse NTP sync state: %s", result)
		logger.Error(err.Error())
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("NTP sync state: %v", enabled)
	return api.Response(http.StatusOK, NtpSyncState{Enabled: enabled}), nil
}

func (s *Controllers) SetNtpSync(ctx context.Context, request api.NtpSyncState) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Set NTP sync request: %v", request)

	if _, err := s.svcUpdate.RunCommand(ctx, "set_ntp_sync", strconv.FormatBool(request.Enabled)); err != nil {
		logger.WithError(err).Error("Failed to set NTP state")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("NTP state changed successfully")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) SetSystemTime(
	ctx context.Context,
	request api.SystemTime,
) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Set time request: %+v", request)

	result, err := s.svcUpdate.RunCommand(ctx, "set_time", request.Timestamp.Format(time.RFC3339))
	if err != nil {
		logger.WithError(err).Error("Failed to set time")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	if result != "ok" {
		err := fmt.Errorf("time sync enabled")
		logger.WithError(err).Error("Cannot set time")
		return api.Response(http.StatusUnprocessableEntity, nil), err
	}

	logger.Infof("Time setting was successful")
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) GetSystemTime(
	ctx context.Context,
) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Get time request")

	result, err := s.svcUpdate.RunCommand(ctx, "get_time", "")
	if err != nil {
		logger.WithError(err).Error("Failed to get time")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	systemTime := strings.Fields(result)
	parsedTime, err := time.Parse(time.RFC3339, systemTime[0])
	if err != nil {
		logger.WithError(err).Error("Failed to parse time")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Successfully get system time: %v", parsedTime)
	return api.Response(http.StatusOK, api.SystemTime{Timestamp: parsedTime}), nil
}
