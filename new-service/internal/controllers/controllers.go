package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/proxy"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcBackend        backend.BackendService
	svcSensorNotifier *components.Notifier[*core.SensorInfoDynamic]
	svcJammerNotifier *components.Notifier[*core.JammerInfoDynamic]
	svcTargetProvider *proxy.Service
}

func NewControllers(
	svcCommon *common.CommonService,
	svcBackend backend.BackendService,
	svcSensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	svcJammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	svcTargetProvider *proxy.Service,
) *Controllers {
	return &Controllers{
		svcCommon:         svcCommon,
		svcBackend:        svcBackend,
		svcSensorNotifier: svcSensorNotifier,
		svcJammerNotifier: svcJammerNotifier,
		svcTargetProvider: svcTargetProvider,
	}
}
