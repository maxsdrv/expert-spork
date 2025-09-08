package bulat

import (
	"context"
	"encoding/json"
	"fmt"

	"dds-provider/internal/generated/bulat"
)

type DeviceState int

const (
	StateInactive DeviceState = iota
	StateActive
	StateOffline
	StateOnline
)

const (
	getObjectAction  = "get_list_car"
	editObjectAction = "p_edit_ts"
)

var stateName = map[DeviceState]string{
	StateInactive: "Inactive",
	StateActive:   "Active",
	StateOffline:  "Offline",
	StateOnline:   "Online",
}

func (s DeviceState) String() string {
	return stateName[s]
}

type ObjectStatus struct {
	DeviceId        int
	EffectiveStatus DeviceState
}

func (s *Service) GetObjects(ctx context.Context) (*api.ObjectsResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Fetching object list")

	payload := map[string]any{
		"action":   getObjectAction,
		"api_type": 0,
	}

	body, err := s.doRequest(ctx, getObjectAction, payload)
	if err != nil {
		return nil, err
	}

	var response api.ObjectsResponse
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Failed to unmarshal response: %v", err)
		return nil, err
	}

	for _, device := range response.Car {
		status := s.updateDeviceStatus(ctx, device)
		logger.Infof("Object %d status: %s", status.DeviceId, status.EffectiveStatus)
	}

	logger.Infof("Successfully fetched object list with %d objects", len(response.Car))
	return &response, nil
}

func (s *Service) EditObject(ctx context.Context, objectId int, objTitle string, stationary int, objLat, objLon float64) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Edit object %d", objectId)

	payload := map[string]any{
		"action":            editObjectAction,
		"lc_id":             objectId,
		"lc_title":          objTitle,
		"lc_stationary":     stationary,
		"lc_stationary_lat": fmt.Sprintf("%.5f", objLat),
		"lc_stationary_lon": fmt.Sprintf("%.5f", objLon),
	}

	_, err := s.doRequest(ctx, editObjectAction, payload)
	if err != nil {
		return err
	}

	logger.Infof("Successfully edited object %d", objectId)
	return nil
}

func (s *Service) updateDeviceStatus(ctx context.Context, object api.ListObject) *ObjectStatus {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Updating device status for object %d", object.LcId)

	status := &ObjectStatus{
		DeviceId: int(object.LcId),
	}

	isActive := object.LcActive == 1
	isOnline := object.LocOnlineFlag == 1

	if isActive && isOnline {
		status.EffectiveStatus = StateOnline
	} else if isActive && object.LocOnlineFlag == 0 {
		status.EffectiveStatus = StateOffline
	} else {
		status.EffectiveStatus = StateInactive
	}

	logger.Debugf("Object %d status set to: %s (active: %v, online: %v)",
		status.DeviceId, status.EffectiveStatus, isActive, isOnline)
	return status
}
