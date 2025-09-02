package parsers

import (
	"encoding/json"
	"fmt"
	"strings"

	"dds-provider/internal/generated/provider_client"
)

const (
	UndefinedJammerMode = "UNDEFINED"
)

func ParseSensorInfo(dataRaw json.RawMessage) (*provider_client.SensorInfo, error) {
	var sensor provider_client.SensorInfo
	if err := json.Unmarshal(dataRaw, &sensor); err != nil {
		if strings.Contains(err.Error(), UndefinedJammerMode) {
			return skippingUndefinedJammerMode(dataRaw)
		}
		return nil, err
	}

	return &sensor, nil
}

func skippingUndefinedJammerMode(dataRaw json.RawMessage) (*provider_client.SensorInfo, error) {
	var rawData map[string]interface{}
	if err := json.Unmarshal(dataRaw, &rawData); err != nil {
		return nil, fmt.Errorf("failed to parse raw data: %w", err)
	}

	if jammerMode, exists := rawData["jammer_mode"]; exists {
		if jammerModeStr, ok := jammerMode.(string); ok && jammerModeStr == UndefinedJammerMode {
			delete(rawData, "jammer_mode")

			modifiedData, err := json.Marshal(rawData)
			if err != nil {
				return nil, fmt.Errorf("failed to re-marshal modified data: %w", err)
			}

			var sensor provider_client.SensorInfo
			if err := json.Unmarshal(modifiedData, &sensor); err != nil {
				return nil, fmt.Errorf("failed to parse modified sensor data: %w", err)
			}

			return &sensor, nil
		}
	}

	var sensor provider_client.SensorInfo
	if err := json.Unmarshal(dataRaw, &sensor); err != nil {
		return nil, fmt.Errorf("failed to parse sensor data: %w", err)
	}

	return &sensor, nil
}

func ParseJammerInfo(dataRaw json.RawMessage) (*provider_client.JammerInfo, error) {
	var jammer provider_client.JammerInfo
	if err := json.Unmarshal(dataRaw, &jammer); err != nil {
		return nil, err
	}
	return &jammer, nil
}

func ParseLicenseStatus(dataRaw json.RawMessage) (*provider_client.LicenseStatus, error) {
	var licenseStatus provider_client.LicenseStatus
	if err := json.Unmarshal(dataRaw, &licenseStatus); err != nil {
		return nil, err
	}

	return &licenseStatus, nil
}
