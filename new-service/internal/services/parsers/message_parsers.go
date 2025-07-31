package parsers

import (
	"encoding/json"

	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

func ParseSensorInfo(dataRaw json.RawMessage) (*dss_target_service.SensorInfo, error) {
	var sensor dss_target_service.SensorInfo
	if err := json.Unmarshal(dataRaw, &sensor); err != nil {
		return nil, err
	}

	return &sensor, nil
}

func ParseJammerInfo(dataRaw json.RawMessage) (*dss_target_service.JammerInfo, error) {
	var jammer dss_target_service.JammerInfo
	if err := json.Unmarshal(dataRaw, &jammer); err != nil {
		return nil, err
	}
	return &jammer, nil
}

func ParseLicenseStatus(dataRaw json.RawMessage) (*dss_target_service.LicenseStatus, error) {
	var licenseStatus dss_target_service.LicenseStatus
	if err := json.Unmarshal(dataRaw, &licenseStatus); err != nil {
		return nil, err
	}

	return &licenseStatus, nil
}
