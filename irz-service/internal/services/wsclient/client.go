package wsclient

import (
	"context"
	"encoding/json"
	"errors"
	"time"

	"github.com/coder/websocket"
	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type WSMessage struct {
	Message string          `json:"message"`
	Data    json.RawMessage `json:"data"`
}

type NotificationHandler interface {
	HandleNotification(ctx context.Context, message string, data json.RawMessage) error
}

type WSNotificationClient struct {
	url                 string
	notificationHandler NotificationHandler
}

func NewWSNotificationClient(url string, notificationHandler NotificationHandler) *WSNotificationClient {
	return &WSNotificationClient{
		url:                 url,
		notificationHandler: notificationHandler,
	}
}

func (c *WSNotificationClient) Start(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Starting connection")

	delay := minBackoff
	for !errors.Is(ctx.Err(), context.Canceled) {
		logger.Infof("Connecting to websocket %s", c.url)
		conn, _, err := websocket.Dial(ctx, c.url, nil)
		if err != nil {
			logger.WithError(err).Errorf("Dial failed, retrying in %v", delay)
			time.Sleep(delay)
			delay = nextBackoffDelay(delay)
			continue
		}
		delay = minBackoff

		logger.Infof("Connected")
		conn.SetReadLimit(1024 * 1024 * 10) // 10MB
		pingCtx, pingCancel := context.WithCancel(ctx)

		go c.monitorPingPong(pingCtx, conn, pingCancel)
		c.updateLoop(pingCtx, conn)

		pingCancel()
		conn.CloseNow()
		logger.Warn("Connection closed")
	}
}

func (c *WSNotificationClient) updateLoop(ctx context.Context, conn *websocket.Conn) {
	logger := logging.WithCtxFields(ctx)

	for {
		_, data, err := conn.Read(ctx)
		if err != nil {
			logger.WithError(err).Warn("Failed to read data from DDS target service")
			break
		}

		var wsMsg WSMessage
		if err = json.Unmarshal(data, &wsMsg); err != nil {
			logger.WithError(err).Error("Unmarshalling message failed")
			continue
		}

		if wsMsg.Message == "" {
			logger.Error("Missing or empty message field")
			continue
		}

		if len(wsMsg.Data) == 0 {
			logger.Error("Missing or empty data field")
			continue
		}

		if err = c.notificationHandler.HandleNotification(ctx, wsMsg.Message, wsMsg.Data); err != nil {
			logger.WithError(err).Errorf("Failed to process message type: %s", wsMsg.Message)
			continue
		}

		logger.Tracef("Successfully processed message type: %s", wsMsg.Message)
	}
	logger.Warn("UpdateLoop ended")
}
