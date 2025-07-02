package controllers

import (
	"dds-provider/internal/core"
	"dds-provider/internal/services/backend"
	"dds-provider/internal/services/common"
	"dds-provider/internal/services/notifier"
)

type Controllers struct {
	svcCommon         *common.CommonService
	svcBackend        *backend.BackendService
	svcJammerNotifier *notifier.NotifierService[*core.JammerInfoDynamic]
}

func New(svcCommon *common.CommonService, svcBackend *backend.BackendService, svcNotifier *notifier.NotifierService[*core.JammerInfoDynamic]) *Controllers {
	return &Controllers{
		svcCommon:         svcCommon,
		svcBackend:        svcBackend,
		svcJammerNotifier: svcNotifier,
	}
}
