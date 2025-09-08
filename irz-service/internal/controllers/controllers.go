package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/core/components"
	"dds-provider/internal/services"
	"dds-provider/internal/services/device_container"
	"dds-provider/internal/services/proxy_service"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcDevStorage     device_container.DeviceContainerService
	svcSensorNotifier *components.Notifier[*core.SensorInfoDynamic]
	svcJammerNotifier *components.Notifier[*core.JammerInfoDynamic]
	svcTargetProvider *proxy_service.Service
}

func NewControllers(svcCommon *common.CommonService,
	svcDevStorage device_container.DeviceContainerService,
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
