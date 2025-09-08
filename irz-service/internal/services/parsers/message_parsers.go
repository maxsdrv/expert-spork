package parsers

import (
	"encoding/json"

	"dds-provider/internal/generated/provider_client"
)

func ParseSensorInfo(dataRaw json.RawMessage) (*provider_client.SensorInfo, error) {
	var sensor provider_client.SensorInfo
	if err := json.Unmarshal(dataRaw, &sensor); err != nil {
		return nil, err
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
