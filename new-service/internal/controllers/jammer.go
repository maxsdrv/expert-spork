package controllers

import (
	"context"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-provider/internal/core"
	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

var controllersError = core.ProviderError()

func (s *Controllers) Jammers(ctx context.Context) (*connect.Response[apiv1.JammersResponse], error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Getting Jammers")

	ids := s.svcDevStorage.ListJammers()

	var jammerInfos []*apiv1.JammerInfo

	for _, jammerId := range ids {
		sensorBase, err := s.svcDevStorage.Jammer(jammerId)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Jammer %s not found", jammerId)
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

	deviceId := core.NewId(jammerId)

	jammerBase, err := s.svcDevStorage.Jammer(deviceId)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Jammer %s not found", deviceId)
		return err
	}

	jammerBandsWriter, ok := jammerBase.(core.JammerBandsWriter)
	if !ok {
		return controllersError("jammer does not support bands setting: %s", deviceId)
	}

	activeBandsList := core.BandList(bandsActive)

	stream, closer, err := s.svcJammerNotifier.Stream(ctx, deviceId)
	if err != nil {
		logger.WithError(controllersError("%v", err)).Errorf("Failed to get jammer configuration: %v", err)
		return err
	}
	defer closer()

	select {
	case jammerDynamic := <-stream:
		if jammerDynamic == nil {
			return controllersError("No dynamic info available for jammer %s", deviceId)
		}

		allSupportedBands := jammerDynamic.Bands.All()

		jammerBands, err := core.NewBands(allSupportedBands, activeBandsList)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to create bands configuration for jammer %s", deviceId)
			return err
		}

		err = jammerBandsWriter.SetJammerBands(jammerBands, duration)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Failed to set jammer bands for device %s", deviceId)
			return err
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

func (s *Controllers) GetGroups(ctx context.Context) ([]*apiv1.JammerGroup, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debug("Getting Groups (decoupled)")

	ids := s.svcDevStorage.ListJammers()
	unique := make(map[string]struct{})
	var groups []*apiv1.JammerGroup

	for _, jammerId := range ids {
		jammerBase, err := s.svcDevStorage.Jammer(jammerId)
		if err != nil {
			logger.WithError(controllersError("%v", err)).Errorf("Jammer %s not found", jammerId)
			continue
		}
		ji := jammerBase.JammerInfo()
		gid := ji.GetGroupId()
		if gid == "" {
			continue
		}
		if _, ok := unique[gid]; ok {
			continue
		}
		unique[gid] = struct{}{}
		gidCopy := gid
		name := gid
		groups = append(groups, &apiv1.JammerGroup{GroupId: &gidCopy, Name: &name})
	}

	return groups, nil
}
