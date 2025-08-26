package bulat

import (
	"context"
	"encoding/json"
	"time"

	api "dds-provider/internal/generated/bulat"
)

const pollInterval = time.Second * 10

func (s *Service) InitEventID(ctx context.Context) (int, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Init event ID")

	payload := map[string]any{
		"action":   "get_list_car",
		"api_type": 1,
		"de_id":    0,
	}

	body, err := s.doRequest(ctx, payload)
	if err != nil {
		logger.Errorf("Init event ID error: %s", err)
		return 0, err
	}

	var response api.EventResponse
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Init event ID unmarshalling error: %s", err)
		return 0, err
	}

	logger.Infof("Initialized event ID: %d", response.Event.DeId)

	return int(response.Event.DeId), nil
}

func (s *Service) PollEvents(ctx context.Context, deID int, onEvent func(api.EventListItem)) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Start polling events from %d", deID)

	eventID := deID

	for {
		select {
		case <-ctx.Done():
			logger.Infof("Stop polling events - context cancelled")
			return
		case <-time.After(pollInterval):
			logger.Debugf("Poll events from %d", eventID)

			payload := map[string]any{
				"action":   "get_list_car",
				"api_type": 1,
				"de_id":    eventID,
			}

			body, err := s.doRequest(ctx, payload)
			if err != nil {
				logger.Errorf("Poll events error: %s", err)
				continue
			}

			var response api.EventResponse
			if err := json.Unmarshal(body, &response); err != nil {
				logger.Errorf("Poll event ID unmarshalling error: %s", err)
				continue
			}

			if len(response.Event.List) == 0 {
				logger.Debugf("No new events found for %d", eventID)
				continue
			}

			logger.Infof("Received %d new events", len(response.Event.List))

			maxEventID := eventID
			for _, ev := range response.Event.List {
				logger.Infof("Processing event: %d - %s", ev.Id, ev.LblRu)
				onEvent(ev)
				if int(ev.Id) > maxEventID {
					maxEventID = int(ev.Id)
				}
			}

			eventID = maxEventID + 1
			logger.Infof("Next poll will start from event ID: %d", eventID)
		}
	}
}
