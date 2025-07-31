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

const (
	pingInterval = 5 * time.Second
	pongInterval = 4 * time.Second
	minBackoff   = 100 * time.Millisecond
	maxBackoff   = 10 * time.Second
)

const (
	fieldMessage = "message"
	fieldData    = "data"
)

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
			logger.WithError(err).Warn("Failed to read data from DDS target proxy")
			break
		}

		var fields map[string]json.RawMessage
		if err = json.Unmarshal(data, &fields); err != nil {
			logger.WithError(err).Error("Unmarshalling message failed")
			continue
		}

		msgTypeRaw, ok := fields[fieldMessage]
		if !ok {
			logger.Errorf("Missing message field %s", fieldMessage)
			continue
		}
		var msgType string
		if err = json.Unmarshal(msgTypeRaw, &msgType); err != nil {
			logger.WithError(err).Error("Unmarshalling message failed")
			continue
		}

		dataRaw, ok := fields[fieldData]
		if !ok {
			logger.Errorf("Missing message field %s", fieldData)
			continue
		}

		if err = c.notificationHandler.HandleNotification(ctx, msgType, dataRaw); err != nil {
			logger.WithError(err).Errorf("Failed to process message type: %s", msgType)
			continue
		}

		logger.Tracef("Successfully processed message type: %s", msgType)
	}
	logger.Warn("UpdateLoop ended")
}

func (c *WSNotificationClient) monitorPingPong(ctx context.Context, conn *websocket.Conn, cancel context.CancelFunc) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Pinging WebSocket %s", c.url)

	pingTicker := time.NewTicker(pingInterval)
	defer pingTicker.Stop()

	for {
		select {
		case <-ctx.Done():
			logger.Debugf("Ping canceled")
			return
		case <-pingTicker.C:
			logger.Tracef("Ping...")
			pongCtx, _ := context.WithTimeout(ctx, pongInterval)
			err := conn.Ping(pongCtx)

			if err != nil {
				logger.WithError(err).Warn("Ping failed")
				cancel()
				return
			}
			logger.Tracef("Pong...")
		}
	}
}

func nextBackoffDelay(prev time.Duration) time.Duration {
	next := prev * 2
	if next > maxBackoff {
		next = maxBackoff
	}
	return next
}
