package controllers

import (
	"context"
	"fmt"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

func (s *Controllers) GetJammers(ctx context.Context) (*connect.Response[apiv1.JammersResponse], error) {
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

	deviceId := core.NewId(jammerId)

	jammerBase, err := s.svcDevStorage.Jammer(deviceId)
	if err != nil {
		logger.WithError(err).Errorf("Failed to get jammer %s", deviceId)
		return err
	}

	jammerBandsWriter, ok := (*jammerBase).(core.JammerBandsWriter)
	if !ok {
		logger.Errorf("Jammer %s does not support band setting", deviceId)
		return fmt.Errorf("jammer %s does not support band setting", deviceId)
	}

	activeBandsList := core.BandList(bandsActive)

	jammerInfoChan, cleanup, err := s.svcJammerNotifier.Stream(ctx, deviceId)
	if err != nil {
		logger.WithError(err).Errorf("Failed to get jammer dynamic info for %s", deviceId)
		return fmt.Errorf("failed to get jammer configuration: %w", err)
	}
	defer cleanup()

	select {
	case jammerDynamic := <-jammerInfoChan:
		if jammerDynamic == nil {
			return fmt.Errorf("no dynamic info available for jammer %s", deviceId)
		}

		allSupportedBands := jammerDynamic.Bands.GetAll()

		jammerBands, err := core.NewBands(allSupportedBands, activeBandsList)
		if err != nil {
			logger.WithError(err).Errorf("Failed to create bands configuration for jammer %s", deviceId)
			return fmt.Errorf("invalid bands configuration: %w", err)
		}

		err = jammerBandsWriter.SetJammerBands(jammerBands, duration)
		if err != nil {
			logger.WithError(err).Errorf("Failed to set jammer bands for device %s", deviceId)
			return fmt.Errorf("failed to set jammer bands: %w", err)
		}

	case <-ctx.Done():
		return ctx.Err()
	}

	logger.Debugf("Successfully set jammer bands for device %s", deviceId)

	return nil
}

func (s *Controllers) JammerInfoDynamic(
	ctx context.Context,
	id string,
	sender func(jammer *apiv1.JammerInfoDynamicResponse) error,
) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("JammerInfoDynamic request")

	jammerInfoChan, closer, err := s.svcJammerNotifier.Stream(ctx, core.NewId(id))
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
