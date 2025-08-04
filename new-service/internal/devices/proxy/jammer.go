package proxy

import (
	"context"
	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/mapping"
	"errors"
)

type Jammer struct {
	id         string
	serviceApi *dss_target_service.JammerAPIService
	info       *dss_target_service.JammerInfo
}

func NewJammer(jammerId string, api *dss_target_service.JammerAPIService, info *dss_target_service.JammerInfo) (*Jammer, error) {
	if jammerId == "" {
		return nil, errors.New("jammer id is required")
	}
	return &Jammer{
		id:         jammerId,
		serviceApi: api,
		info:       info,
	}, nil
}

func (j *Jammer) JammerInfo() apiv1.JammerInfo {
	return *mapping.ConvertToAPIJammerInfo(*j.info)
}

func (j *Jammer) SetDisabled(disabled bool) {
}

func (j *Jammer) SetPosition(position *core.GeoPosition) error {
	return nil
}

func (j *Jammer) SetPositionMode(mode core.GeoPositionMode) error {
	return nil
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
