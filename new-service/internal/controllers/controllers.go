package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/devices/proxy"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/notifier"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcBackend        backend.BackendService
	svcJammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic]
	svcSensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic]
	svcTargetProvider *proxy.Service
}

func New(
	svcCommon *common.CommonService,
	svcBackend backend.BackendService,
	svcNotifier *notifier.NotifierService[*core.JammerInfoDynamic],
	svcSensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic],
	svcTargetProvider *proxy.Service,
) *Controllers {
	return &Controllers{
		svcCommon:         svcCommon,
		svcBackend:        svcBackend,
		svcJammerNotifier: svcNotifier,
		svcSensorNotifier: svcSensorNotifier,
		svcTargetProvider: svcTargetProvider,
	}
}
