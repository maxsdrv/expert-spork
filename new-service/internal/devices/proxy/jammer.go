package proxy

import (
	"context"

	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy/mapping"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

var proxyError = core.ProviderError()

type Jammer struct {
	id         string
	serviceApi *provider_client.JammerAPIService
	deviceApi  *provider_client.DeviceAPIService
	info       *provider_client.JammerInfo
}

func NewJammer(jammerId string, api *provider_client.APIClient, info *provider_client.JammerInfo) (*Jammer, error) {
	if jammerId == "" {
		return nil, proxyError("jammer id is empty")
	}
	return &Jammer{
		id:         jammerId,
		serviceApi: api.JammerAPI,
		deviceApi:  api.DeviceAPI,
		info:       info,
	}, nil
}

func (j *Jammer) JammerInfo() apiv1.JammerInfo {
	jammerCoreInfo := mapping.ConvertToJammerInfo(*j.info)
	return *jammerCoreInfo.ToAPI()
}

func (j *Jammer) SetDisabled(disabled bool) error {
	setDisabledReq := provider_client.SetDisabledRequest{
		Id:       j.id,
		Disabled: disabled,
	}

	_, err := j.deviceApi.SetDisabled(context.Background()).
		SetDisabledRequest(setDisabledReq).
		Execute()

	if err != nil {
		return proxyError("SetDisabled: %s, %v", j.id, err)
	}

	return nil
}

func (j *Jammer) SetPosition(position core.GeoPosition) error {
	dssPosition := mapping.ConvertToProviderGeoPosition(position)
	setPositionReq := provider_client.SetPositionRequest{
		Id:       j.id,
		Position: dssPosition,
	}

	_, err := j.deviceApi.SetPosition(context.Background()).
		SetPositionRequest(setPositionReq).
		Execute()

	if err != nil {
		return proxyError("SetPosition: %s, %v", j.id, err)
	}

	return nil
}

func (j *Jammer) SetPositionMode(mode core.GeoPositionMode) error {
	dssMode, err := mapping.ConvertToProviderGeoPositionMode(mode)
	if err != nil {
		return err
	}
	setPositionModeReq := provider_client.SetPositionModeRequest{
		Id:           j.id,
		PositionMode: dssMode,
	}

	_, err = j.deviceApi.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	if err != nil {
		return proxyError("SetPositionMode: %s, %v", j.id, err)
	}

	return nil
}

func (j *Jammer) SetJammerBands(bands core.JammerBands, duration uint) error {

	activeBands := bands.Active()

	setJammerBandsReq := provider_client.SetJammerBandsRequest{
		Id:          j.id,
		BandsActive: activeBands,
		Duration:    int32(duration),
	}

	_, err := j.serviceApi.SetJammerBands(context.Background()).
		SetJammerBandsRequest(setJammerBandsReq).
		Execute()

	if err != nil {
		return proxyError("SetJammerBands: %s, %v", j.id, err)
	}

	return nil
}
