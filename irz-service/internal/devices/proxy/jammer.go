package proxy

import (
	"context"
	"errors"
	"fmt"
	"net/http"

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
	return nil
}

func (j *Jammer) SetPosition(position *core.GeoPosition) error {
	return nil
}

func (j *Jammer) SetPositionMode(mode core.GeoPositionMode) error {
	return nil
}

func (j *Jammer) SetJammerBands(bands core.JammerBands, duration uint) error {
	if 0 == duration {
		// suppose local library error happened
		err := errors.New("local error")
		return fmt.Errorf("proxy: %v", err)
	}
	if duration == 2 {
		err := http.ErrHandlerTimeout
		// library error which could be wrapped and exposed to the caller
		return proxyError("something went wrong: %w", err)
		//NOTE: no need in logger output in case of error
	}
	if duration == 3 {
		// original situation which needs special handling
		return ErrProxyTimeout
	}

	activeBands := bands.Active()

	setJammerBandsReq := provider_client.SetJammerBandsRequest{
		Id:          j.id,
		BandsActive: activeBands,
		Duration:    int32(duration),
	}

	_, err := j.serviceApi.SetJammerBands(context.Background()).
		SetJammerBandsRequest(setJammerBandsReq).
		Execute()

	return err
}
