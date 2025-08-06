package proxy

import (
	"context"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/mapping"
)

type Jammer struct {
	id         string
	serviceApi *dss_target_service.JammerAPIService
	deviceApi  *dss_target_service.DeviceAPIService
	info       *dss_target_service.JammerInfo
}

func NewJammer(jammerId string, api *dss_target_service.APIClient, info *dss_target_service.JammerInfo) (*Jammer, error) {
	if jammerId == "" {
		return nil, proxyError("jammer id is required")
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

func (j *Jammer) SetDisabled(disabled bool) {
	setDisabledReq := dss_target_service.SetDisabledRequest{
		Id:       j.id,
		Disabled: disabled,
	}

	_, _ = j.deviceApi.SetDisabled(context.Background()).
		SetDisabledRequest(setDisabledReq).
		Execute()
}

func (j *Jammer) SetPosition(position *core.GeoPosition) error {
	if position == nil {
		return proxyError("position is required")
	}

	dssPosition := mapping.ConvertToDSSGeoPosition(*position)
	setPositionReq := dss_target_service.SetPositionRequest{
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
	setPositionModeReq := dss_target_service.SetPositionModeRequest{
		Id:           j.id,
		PositionMode: dssMode,
	}

	_, err := j.deviceApi.SetPositionMode(context.Background()).
		SetPositionModeRequest(setPositionModeReq).
		Execute()

	return handleJammerError("SetPositionMode", j.id, err)
}

func (j *Jammer) SetJammerBands(bands core.JammerBands, duration int32) error {

	activeBands := bands.GetActive()

	setJammerBandsReq := dss_target_service.SetJammerBandsRequest{
		Id:          j.id,
		BandsActive: activeBands,
		Duration:    duration,
	}

	_, err := j.serviceApi.SetJammerBands(context.Background()).
		SetJammerBandsRequest(setJammerBandsReq).
		Execute()

	return handleJammerError("SetJammerBands", j.id, err)
}
