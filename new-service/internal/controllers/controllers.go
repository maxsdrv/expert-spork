package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/device_storage"
	"dds-provider/internal/services/proxy_service"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcDevStorage     device_storage.DeviceStorageService
	svcJammerNotifier *components.Notifier[*core.JammerInfoDynamic]
	svcSensorNotifier *components.Notifier[*core.SensorInfoDynamic]
	svcTargetProvider *proxy_service.Service
}

func NewControllers(
	svcCommon *common.CommonService,
	svcDevStorage device_storage.DeviceStorageService,
	svcSensorNotifier *components.Notifier[*core.SensorInfoDynamic],
	svcJammerNotifier *components.Notifier[*core.JammerInfoDynamic],
	svcTargetProvider *proxy_service.Service,
) *Controllers {
	return &Controllers{
		svcCommon:         svcCommon,
		svcDevStorage:     svcDevStorage,
		svcSensorNotifier: svcSensorNotifier,
		svcJammerNotifier: svcJammerNotifier,
		svcTargetProvider: svcTargetProvider,
	}
}
