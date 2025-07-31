package controllers

import (
	"context"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

func (s *Controllers) GetJammers(ctx context.Context) (*connect.Response[apiv1.JammersResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Getting Jammers")

	ids := s.svcBackend.ListJammers()

	var jammerInfos []*apiv1.JammerInfo

	for _, jammerId := range ids {
		sensorBase, err := s.svcBackend.Jammer(jammerId)
		if err != nil {
			logger.WithError(err).Errorf("Failed to get jammer %s", jammerId)
			continue
		}

		jammerInfo := (*sensorBase).JammerInfo()
		jammerInfos = append(jammerInfos, &jammerInfo)
	}

	return connect.NewResponse(&apiv1.JammersResponse{
		JammerInfos: jammerInfos,
	}), nil

}

func (s *Controllers) SetJammerBands(
	ctx context.Context,
	jammerId string,
	bandsActive []string,
	duration int32,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("SetJammerBands request")

	return nil
}

func (s *Controllers) JammerInfoDynamic(
	ctx context.Context,
	id string,
	sender func(jammer *apiv1.JammerInfoDynamicResponse) error,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("JammerInfoDynamic request")

	jammerInfoChan, cleanup, err := s.svcJammerNotifier.Stream(ctx, core.NewId(id))
	if err != nil {
		return err
	}
	defer cleanup()

	for jammer := range jammerInfoChan {
		if err = sender(jammer.ToAPI()); err != nil {
			break
		}
	}

	return err
}
