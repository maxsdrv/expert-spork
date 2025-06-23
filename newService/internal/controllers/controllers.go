package controllers

import (
	"dds-provider/internal/services"
)

type Controllers struct {
	svcCommon *common.CommonService
}

func New(svcBulat *common.CommonService) *Controllers {
	return &Controllers{
		svcCommon: svcBulat,
	}
}
