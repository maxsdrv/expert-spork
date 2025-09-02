package proxy

import (
	"context"

	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy/mapping"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/provider_client"
)

type Jammer struct {
	id         string
	serviceApi *provider_client.JammerAPIService
	deviceApi  *provider_client.DeviceAPIService
	info       *provider_client.JammerInfo
}

func NewJammer(jammerId string, api *provider_client.APIClient, info *provider_client.JammerInfo) *Jammer {
	return &Jammer{
		id:         jammerId,
		serviceApi: api.JammerAPI,
		deviceApi:  api.DeviceAPI,
		info:       info,
	}
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

	return handleJammerError("SetDisabled", j.id, err)
}

func (j *Jammer) SetPosition(position core.GeoPosition) error {
	dssPosition := mapping.ConvertToDSSGeoPosition(position)
	setPositionReq := provider_client.SetPositionRequest{
		Id:       j.id,
		Position: dssPosition,
	}

	_, err := j.deviceApi.SetPosition(context.Background()).
		SetPositionRequest(setPositionReq).
		Execute()

	return handleJammerError("SetPosition", j.id, err)
}

func (j *Jammer) SetPositionMode(mode core.GeoPositionMode) error {
	dssMode := mapping.ConvertToDSSGeoPositionMode(mode)
	setPositionModeReq := provider_client.SetPositionModeRequest{
		Id:           j.id,
		PositionMode: dssMode,
	}

	_, err := j.deviceApi.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	return handleJammerError("SetPositionMode", j.id, err)
}

func (j *Jammer) SetJammerBands(bands core.JammerBands, duration uint) error {

	activeBands := bands.GetActive()

	setJammerBandsReq := provider_client.SetJammerBandsRequest{
		Id:          j.id,
		BandsActive: activeBands,
		Duration:    int32(duration),
	}

	_, err := j.serviceApi.SetJammerBands(context.Background()).
		SetJammerBandsRequest(setJammerBandsReq).
		Execute()

	return handleJammerError("SetJammerBands", j.id, err)
}
