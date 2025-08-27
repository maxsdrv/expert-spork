package controllers

import (
	"context"
	"net/http"

	"dds-control-api/internal/generated/api"
)

func (s *Controllers) SetConfig(
	ctx context.Context,
	type_ api.ConfigType, configFileData api.ConfigFileData) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("SetConfig: type: %+v", type_)
	logger.Tracef("SetConfig: fileContent: %+v", configFileData.Data)

	var command string
	if type_ == api.CONFIGTYPE_DEVICES {
		command = "update_devices_config"
	} else if type_ == api.CONFIGTYPE_SYSTEM {
		command = "update_system_config"
	}

	result, err := s.svcUpdate.RunCommand(ctx, command, configFileData.Data)
	if err != nil {
		logger.WithError(err).Error("Set config error")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Debugf("SetConfig: %s", result)
	return api.Response(http.StatusOK, nil), nil
}

func (s *Controllers) GetConfig(
	ctx context.Context, type_ api.ConfigType) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("GetConfig request")

	var command string
	if type_ == api.CONFIGTYPE_DEVICES {
		command = "read_devices_config"
	} else if type_ == api.CONFIGTYPE_SYSTEM {
		command = "read_system_config"
	}

	result, err := s.svcUpdate.RunCommand(ctx, command, "")
	if err != nil {
		logger.WithError(err).Error("Get config error")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Debugf("GetConfig: %s", result)
	return api.Response(http.StatusOK, api.ConfigFileData{Data: result}), nil
}
