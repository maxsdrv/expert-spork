package controllers

import (
	"context"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

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

	deviceId, err := core.NewId(id)
	if err != nil {
		return err
	}

	jammerInfoChan, closer, err := s.svcJammerNotifier.Stream(ctx, deviceId)
	if err != nil {
		return err
	}
	defer closer()

	for jammer := range jammerInfoChan {
		if err = sender(jammer.ToAPI()); err != nil {
			break
		}
	}

	return err
}
