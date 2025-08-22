package proxy_service

import (
	"context"

	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
)

func NewNotifiers(ctx context.Context) (
	*components.Notifier[*core.SensorInfoDynamic],
	*components.Notifier[*core.JammerInfoDynamic],
) {
	sensorNotifier := components.NewNotifier[*core.SensorInfoDynamic](ctx)
	jammerNotifier := components.NewNotifier[*core.JammerInfoDynamic](ctx)
	return sensorNotifier, jammerNotifier
}
