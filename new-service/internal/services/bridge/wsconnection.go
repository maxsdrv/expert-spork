package bridge

import (
	"context"
	"errors"
	"time"

	"github.com/coder/websocket"
)

const (
	pingInterval = 5 * time.Second
	pongInterval = 4 * time.Second
	minBackoff   = 100 * time.Millisecond
	maxBackoff   = 10 * time.Second
)

func (s *Device) startConnection(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Starting connection")

	delay := minBackoff
	for !errors.Is(ctx.Err(), context.Canceled) {
		logger.Infof("Connecting to websocket %s", s.urlWs)
		conn, _, err := websocket.Dial(ctx, s.urlWs, nil)
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

		go s.monitorPingPong(pingCtx, conn, pingCancel)
		s.updateLoop(pingCtx, conn)

		pingCancel()
		conn.CloseNow()
		logger.Warn("Connection closed")
	}
}

func (s *Device) monitorPingPong(ctx context.Context, conn *websocket.Conn, cancel context.CancelFunc) {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Pinging WebSocket %s", s.urlWs)

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
