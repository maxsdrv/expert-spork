package controllers

import (
	"context"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

func (s *Controllers) Jammers(ctx context.Context) (*connect.Response[apiv1.JammersResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Getting Jammers")

	ids := s.svcDevStorage.ListJammers()

	var jammerInfos []*apiv1.JammerInfo

	for _, jammerId := range ids {
		sensorBase, err := s.svcDevStorage.Jammer(jammerId)
		if err != nil {
			logger.WithError(err).Errorf("Failed to get jammer %s", jammerId)
			continue
		}

		jammerInfo := sensorBase.JammerInfo()
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
	duration uint,
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

	stream, closer, err := s.svcJammerNotifier.Stream(ctx, core.NewId(id))
	if err != nil {
		return err
	}
	defer closer()

	for jammer := range stream {
		if err = sender(jammer.ToAPI()); err != nil {
			break
		}
	}

	return err
}
