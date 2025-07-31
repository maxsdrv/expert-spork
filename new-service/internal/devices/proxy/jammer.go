package proxy

import (
	"context"
	"errors"
	"fmt"
	"net/http"

	"dds-provider/internal/core"
	apiv1 "dds-provider/internal/generated/api/proto"
	"dds-provider/internal/generated/radariq-client/dss_target_service"
	"dds-provider/internal/services/wsclient"
)

type Jammer struct {
	id         string
	serviceApi *dss_target_service.JammerAPIService
	//mapper     *wsclient.SensorDataMapper
	info *dss_target_service.JammerInfo
}

func NewJammer(jammerId string, api *dss_target_service.JammerAPIService, info *dss_target_service.JammerInfo) *Jammer {
	return &Jammer{
		id:         jammerId,
		serviceApi: api,
		//mapper:     wsclient.NewSensorDataMapper(),
		info: info,
	}
}

func (j *Jammer) JammerInfo() apiv1.JammerInfo {
	wsclient.SensorDataMapper.ConvertToAPISensorInfo(*j.info)
	return *j.mapper.ConvertToAPIJammerInfo(*j.info)
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

	activeBands := bands.GetActive()

	setJammerBandsReq := dss_target_service.SetJammerBandsRequest{
		Id:          j.id,
		BandsActive: activeBands,
		Duration:    duration,
	}

	_, err := j.serviceApi.SetJammerBands(context.Background()).
		SetJammerBandsRequest(setJammerBandsReq).
		Execute()

	return err
}
