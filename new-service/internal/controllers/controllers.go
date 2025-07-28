package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/notifier"
	"dds-provider/internal/services/service"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcBackend        backend.BackendService
	svcJammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic]
	svcSensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic]
	svcTargetProvider *service.Service
}

func New(
	svcCommon *common.CommonService,
	svcBackend backend.BackendService,
	svcNotifier *notifier.NotifierService[*core.JammerInfoDynamic],
	svcSensorNotifier *notifier.NotifierService[*core.SensorInfoDynamic],
	svcTargetProvider *service.Service,
) *Controllers {
	return &Controllers{
		svcCommon:         svcCommon,
		svcBackend:        svcBackend,
		svcJammerNotifier: svcNotifier,
		svcSensorNotifier: svcSensorNotifier,
		svcTargetProvider: svcTargetProvider,
	}
}
