package bulat

import (
	"context"
	"encoding/json"
	"fmt"
)

type DeviceState int

const (
	StateInactive DeviceState = iota
	StateActive
	StateOffline
	StateOnline
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

type ObjectResponse struct {
	Status      int          `json:"status"`
	ListObjects []ListObject `json:"car"`
}

type ListObject struct {
	LcID          int                `json:"lc_id"`            // Object id
	LcTitle       string             `json:"lc_title"`         // Object name
	LcActive      int                `json:"lc_active"`        // Device active=1, inactive=0
	LocCourse     int                `json:"loc_course"`       // Object course
	LocCtrl       int                `json:"loc_ctrl"`         // Controller id which the data was received
	LocLat        *float64           `json:"loc_lat"`          // Latitude
	LocLon        *float64           `json:"loc_lon"`          // Longitude
	LocOnlineFlag int                `json:"loc_online_flag"`  // Device online=1, offline=0
	LocDTSecDelta int                `json:"loc_dt_sec_delta"` // Time in seconds from location data
	LocDT         int64              `json:"loc_dt"`           // Location data time
	LocSDT        int64              `json:"loc_sdt"`          // Time of communication with the tracker
	LocAddress    string             `json:"loc_address"`      //
	Group         int                `json:"group"`            // Group object id(0=without group)
	Title         string             `json:"title"`            // Zone name
	DevicesData   []DeviceDataObject `json:"devices_data"`
}

type DeviceDataObject struct {
	Id   int    `json:"id"`
	Imei string `json:"imei"`
}

func (s *Service) GetObjects(ctx context.Context) (*ObjectResponse, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Fetching object list")

	payload := map[string]interface{}{
		"action":   "get_list_car",
		"api_type": 0,
	}

	body, err := s.doRequest(ctx, payload)
	if err != nil {
		return nil, err
	}

	var response ObjectResponse
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Failed to unmarshal response: %v", err)
		return nil, err
	}

	for _, device := range response.ListObjects {
		status := s.updateDeviceStatus(ctx, device)
		logger.Infof("Object %d status: %s\n", status.DeviceId, status.EffectiveStatus)
	}

	logger.Infof("Successfully fetched object list")
	return &response, nil
}

func (s *Service) EditObject(ctx context.Context, objectId int, objTitle string, stationary int, objLat, objLon float64) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Edit object")

	payload := map[string]interface{}{
		"action":            "p_edit_ts",
		"lc_id":             objectId,
		"lc_title":          objTitle,
		"lc_stationary":     stationary,
		"lc_stationary_lat": fmt.Sprintf("%.5f", objLat),
		"lc_stationary_lon": fmt.Sprintf("%.5f", objLon),
	}

	_, err := s.doRequest(ctx, payload)
	if err != nil {
		return err
	}

	logger.Infof("Successfully edited object %d", objectId)
	return nil
}

func (s *Service) updateDeviceStatus(ctx context.Context, object ListObject) *ObjectStatus {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Updating device status")

	status := &ObjectStatus{
		DeviceId: object.LcID,
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

	return status
}
