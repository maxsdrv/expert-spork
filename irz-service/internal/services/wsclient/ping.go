package wsclient

import (
	"context"
	"time"

	"github.com/coder/websocket"
)

const (
	pingInterval = 5 * time.Second
	pongInterval = 4 * time.Second
	minBackoff   = 100 * time.Millisecond
	maxBackoff   = 10 * time.Second
)

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
