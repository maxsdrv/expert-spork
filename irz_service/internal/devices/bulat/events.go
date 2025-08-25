package bulat

import (
	"context"
	"encoding/json"
	"time"
)

const pollInterval = time.Second * 10

type EventResponse struct {
	Status int         `json:"status"`  // 1 if the request was successful
	UID    int         `json:"uid"`     // User ID of the requester
	OID    int         `json:"oid"`     // Organization ID
	Now    int64       `json:"now"`     // Current timestamp (test_event_server time)
	SysMsg string      `json:"sys_msg"` // Number of system messages
	Event  EventObject `json:"event"`   // Event data container
}

type EventObject struct {
	DEID int             `json:"de_id"` // Minimum event ID from which new events are fetched
	List []EventListItem `json:"list"`  // List of individual event entries
}

type EventListItem struct {
	Car     string   `json:"car"`     // Object name
	CarID   int      `json:"car_id"`  // Object ID
	Devices []string `json:"devices"` // List of device IMEIs (hardware numbers)
	DT      int64    `json:"dt"`      // Timestamp of event occurrence
	Lbl     int      `json:"lbl"`     // Internal event name ID
	LblRu   string   `json:"lbl_ru"`  // Event name (in Russian, human-readable)
	ID      int      `json:"id"`      // Unique ID of this event
	Type    int      `json:"type"`    // Event type (0 = analytical, 1 = hardware)
	Lat     float64  `json:"lat"`     // Latitude where the event occurred
	Lon     float64  `json:"lon"`     // Longitude where the event occurred
	Radius  int      `json:"radius"`  // Radius of uncertainty (in meters) around Lat/Lon
	Icon    string   `json:"icon"`    // Event color (hex string, e.g., "FF0000")
	Title   string   `json:"title"`   // Custom name of the event
	Absent  int      `json:"absent"`  // 1 if no events present, 0 otherwise
	Val     string   `json:"val"`     // Parameter value associated with the event (e.g., "UAV on 4400MHz with RSSI: -78")
}

func (s *Service) InitEventID(ctx context.Context) (int, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Init event ID")

	payload := map[string]interface{}{
		"action":   "get_list_car",
		"api_type": 1,
		"de_id":    0,
	}

	body, err := s.doRequest(ctx, payload)
	if err != nil {
		logger.Errorf("Init event ID error: %s", err)
		return 0, err
	}

	var response EventResponse
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Init event ID unmarshalling error: %s", err)
		return 0, err
	}

	logger.Infof("Initialized event ID: %d", response.Event.DEID)
	return response.Event.DEID, nil
}

func (s *Service) PollEvents(ctx context.Context, deID int, onEvent func(EventListItem)) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Poll events")

	eventID := deID

	for {
		select {
		case <-ctx.Done():
			logger.Infof("Stop polling events")
			return
		case <-time.After(pollInterval):
			payload := map[string]interface{}{
				"action":   "get_list_car",
				"api_type": 1,
				"de_id":    eventID,
			}

			body, err := s.doRequest(ctx, payload)
			if err != nil {
				logger.Errorf("Poll events error: %s", err)
				continue
			}

			var response EventResponse
			if err := json.Unmarshal(body, &response); err != nil {
				logger.Errorf("Poll event ID unmarshalling error: %s", err)
				continue
			}

			for _, ev := range response.Event.List {
				onEvent(ev)
				if ev.ID >= eventID {
					eventID = ev.ID + 1
				}
			}
		}
	}
}
