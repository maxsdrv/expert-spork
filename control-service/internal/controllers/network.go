package controllers

import (
	"context"
	"fmt"
	"net"
	"net/http"
	"strings"

	"dds-control-api/internal/generated/api"
	"dds-control-api/internal/services/network"
)

func (s *Controllers) GetNetworkSettings(ctx context.Context) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("GetNetworkSettings request")

	result, err := s.svcUpdate.RunCommand(ctx, "get_network_settings", "")
	if err != nil {
		logger.WithError(err).Errorf("Failed to get network settings")
		return api.Response(http.StatusInternalServerError, nil), err
	}
	logger.Debug("Result: " + result)

	settings := strings.Fields(result)
	if len(settings) < 2 {
		err := fmt.Errorf("invalid network settings count: %d", len(settings))
		logger.WithError(err).Errorf("Failed to parse network settings")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	var networkSettings api.NetworkSettings

	ip, mask, err := net.ParseCIDR(settings[0])
	if err != nil {
		logger.WithError(err).Errorf("Failed to parse network data")
		return api.Response(http.StatusInternalServerError, nil), err
	}
	prefix, _ := mask.Mask.Size()
	networkSettings.Address = ip.String()
	networkSettings.Prefix = int32(prefix)

	gateway := settings[1]
	if gateway == "-" {
		gateway = ""
	} else {
		networkSettings.Gateway = gateway
	}

	if len(settings) > 2 {
		networkSettings.DnsNameservers = settings[2:]
	}

	logger.Infof("Successfully retrieved network settings: %v", settings)
	return api.Response(http.StatusOK, networkSettings), nil
}

func (s *Controllers) SetNetworkSettings(ctx context.Context, settings api.NetworkSettings) (api.ImplResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("SetNetworkSettings request: %v", settings)

	gateway := settings.Gateway
	if gateway == "" {
		gateway = "-"
	}

	fileData, err := s.svcUpdate.RunCommand(ctx, "get_netplan_config_file", "")
	if err != nil {
		logger.WithError(err).Errorf("Failed to get netplan config file")
		return api.Response(http.StatusInternalServerError, nil), err
	}
	fileDataArray := strings.SplitN(fileData, " ", 3)
	netInterface := fileDataArray[0]
	fileName := fileDataArray[1]
	fileContent := fileDataArray[2]

	fileContent, err = network.ModifyNetplanConfig(ctx, fileContent, netInterface, settings)
	if err != nil {
		logger.WithError(err).Errorf("Failed to modify netplan config file")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	_, err = s.svcUpdate.RunCommand(ctx, "set_netplan_config_file", fileName+"\n"+fileContent)
	if err != nil {
		logger.WithError(err).Error("Failed to set netplan config file")
		return api.Response(http.StatusInternalServerError, nil), err
	}

	logger.Infof("Network settings updated successfully")
	return api.Response(http.StatusOK, nil), nil
}
